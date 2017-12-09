#include "FileSenderHandler.h"
#include "NetworkManager.h"
#include "Connection.h"
#include <QCoreApplication>
#include <QDebug>
#include "Messages/ShareRequestMsg.h"
#include "Messages/ShareResponseMsg.h"
#include "Messages/FileTransferMsg.h"
#include "Messages/FilePartTransferMsg.h"
#include "Utils.h"
#include <QHash>
#include <QPair>
#include <QFileInfo>
#include <QDir>
#include <QFileInfoList>
#include <QUuid>
#include <QtMath>
#include <QThread>


FileSenderHandler::FileSenderHandler(Connection* conn, const QStringList& files, QObject *p)
    : FileHandlerBase(conn, "", p)
    , mRootFiles(files)
    , mCurrentRootFileIndex(0)
    , mCurrentFileIndex(0)
    , mFile(0)
    , mIndexOfBasePath(-1)
    , mRootTotalSize(0)
{
    connect(this, &FileSenderHandler::transferStatusChanged, [=](TransferStatusFlag::ControlStatus status) {
        switch (status) {
            case TransferStatusFlag::Running:
                if (mAllFiles.count()) {
                    sendFilePart(mCurrentSeqNo + 1);
                }
            break;
        }
    });
}


void FileSenderHandler::handleThreadStarting()
{
    transferStatus(TransferStatusFlag::Running);
    sendRootFile();
}


void FileSenderHandler::parseFile(const QFileInfo &fi)
{
    if (fi.isDir()) {
        QDir dir(fi.absoluteFilePath());
        QFileInfoList fis = dir.entryInfoList(QDir::AllDirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot);
        foreach (QFileInfo fi, fis) {
            parseFile(fi);
        }
    }
    else {
        mRootTotalSize += fi.size();
        mAllFiles.append(fi);
    }
}


void FileSenderHandler::sendRootFile()
{
    // We need to merge all root files into one, so that a single transfer handles all the files
    mAllFiles.clear();
    if (mCurrentRootFileIndex < mRootFiles.length()) {
        QFileInfo fi(mRootFiles.at(mCurrentRootFileIndex));
        mRootTotalSize = 0;
        parseFile(fi);
        mIndexOfBasePath = fi.isDir() ? fi.absoluteFilePath().lastIndexOf(QDir(fi.absoluteFilePath()).dirName()) : -1;
        mCurrentFileIndex = 0;
        FileTransferHeaderInfoMsg* msg = new FileTransferHeaderInfoMsg(transferId(), fi.fileName(), mAllFiles.length(), mRootTotalSize);
        emit sendingRootFile(mConnection, msg, fi.absolutePath());
        emit sendMsg(msg);
        sendFile();
    }
    else {
        transferStatus(TransferStatusFlag::Finished);
        emit transferDone();
        exit();
        this->deleteLater();
    }
}


void FileSenderHandler::sendFile()
{
    if (mCurrentFileIndex < mAllFiles.length()) {
        QFileInfo fi = mAllFiles.at(mCurrentFileIndex);
        emit startingFile(mConnection, fi.absoluteFilePath());
        mFileUuid = QUuid::createUuid().toString();
        FileTransferMsg* msg = new FileTransferMsg(transferId(),  mFileUuid, fi.fileName());
        if (mIndexOfBasePath >= 0) {
            QString filename = fi.absoluteFilePath();
            int index = filename.lastIndexOf(QDir(filename).dirName());
            QString basePath = filename.mid(mIndexOfBasePath, index - mIndexOfBasePath - 1);
            msg->basePath(basePath);
        }
        else {
            msg->basePath("");
        }
        msg->size(fi.size());
        msg->fileNo(mCurrentFileIndex + 1);
        mTotalSeqCount = qCeil(((qreal)msg->size())/MSG_LEN);
        msg->seqCount(mTotalSeqCount);
        emit sendMsg(msg);
    }
    else {
        mCurrentRootFileIndex++;
        sendRootFile();
    }
}


void FileSenderHandler::sendFilePart(int seqNo)
{
    if (transferStatus() == TransferStatusFlag::Running) {
        mCurrentSeqNo = seqNo;
        if (seqNo < mTotalSeqCount) {
            QByteArray data = mFile->read(MSG_LEN);
            FilePartTransferMsg* msg = new FilePartTransferMsg(transferId(), mFileUuid, mCurrentFileIndex + 1, seqNo, data.length(), data);
            emit sendMsg(msg);
        }
        else {
            mFile->close();
            mFile->deleteLater();
            mCurrentFileIndex++;
            sendFile();
        }
    }
}


void FileSenderHandler::handleMessageComingFrom(Connection *sender, Message *msg)
{
    if (sender == mConnection) {
        if (msg->typeId() == FileTransferAckMsg::TypeID) {
            FileTransferAckMsg* ackMsg = qobject_cast<FileTransferAckMsg*>(msg);
            if (ackMsg->uuid() == mFileUuid) {
                emit fileSent(mConnection, ackMsg);
                mFile = new QFile(mAllFiles.at(mCurrentFileIndex).absoluteFilePath());
                qDebug() << "sending file "  << mFile->fileName();
                if (mFile->open(QFile::ReadOnly)) {
                    sendFilePart(0);
                }
            }
            ackMsg->deleteLater();
        }
        else if (msg->typeId() == FilePartTransferAckMsg::TypeID) {
            FilePartTransferAckMsg* ackMsg = qobject_cast<FilePartTransferAckMsg*>(msg);
            if (ackMsg->uuid() == mFileUuid) {
                emit filePartSent(mConnection, ackMsg);
                sendFilePart(ackMsg->seqNo() + 1);
            }
            ackMsg->deleteLater();
        }
    }
}

