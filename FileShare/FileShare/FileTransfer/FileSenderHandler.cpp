#include "FileSenderHandler.h"
#include "NetworkManager.h"
#include "Connection.h"
#include <QCoreApplication>
#include <QDebug>
#include "Messages/ShareRequestMsg.h"
#include "Messages/ShareResponseMsg.h"
#include "Messages/FileTransferMsg.h"
#include "Messages/FilePartTransferMsg.h"
#include "FileTransferManager.h"
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
    , mFailedItem(0)
    , mRootFiles(files)
    , mCurrentRootFileIndex(0)
    , mCurrentFileIndex(0)
    , mFile(0)
    , mIndexOfBasePath(-1)
    , mRootTotalSize(0)
    , mRootProgressSize(0)
    , mCurrentSeqNo(-1)
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


FileSenderHandler::FileSenderHandler(Connection* conn, TransferFailedItem* item, QObject *p)
    : FileHandlerBase(conn, item->transferId(), p)
    , mFailedItem(item)
    , mCurrentRootFileIndex(item->rootFileIndex())
    , mCurrentFileIndex(item->fileIndex())
    , mFile(0)
    , mIndexOfBasePath(-1)
    , mRootTotalSize(0)
    , mRootProgressSize(item->progressSize())
    , mCurrentSeqNo(item->seqIndex())
{
    for (int i = 0; i < item->countrootFiles(); i++) {
        mRootFiles.append(item->itemrootFilesAt(i));
    }

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
    mAllFiles.clear();
    if (mCurrentRootFileIndex < mRootFiles.length()) {
        QFileInfo fi(mRootFiles.at(mCurrentRootFileIndex));
        mRootTotalSize = 0;
        parseFile(fi);
        mIndexOfBasePath = fi.isDir() ? fi.absoluteFilePath().lastIndexOf(QDir(fi.absoluteFilePath()).dirName()) : -1;
        mCurrentFileIndex = mFailedItem? mFailedItem->fileIndex() : 0;
        FileTransferHeaderInfoMsg* msg = new FileTransferHeaderInfoMsg(transferId(), fi.fileName(),
                                                                       mAllFiles.length(), mFailedItem ? mFailedItem->fileIndex() : 0,
                                                                       mRootTotalSize, mFailedItem ? mFailedItem->progressSize() : 0);

        emit sendingRootFile(mConnection, msg, fi.absolutePath());
        emit sendMsg(msg);
    }
    else {
        destroyMyself(TransferStatusFlag::Finished);
    }
}


void FileSenderHandler::sendFile()
{
    if (mCurrentFileIndex < mAllFiles.length()) {
        QFileInfo fi = mAllFiles.at(mCurrentFileIndex);
        emit startingFile(mConnection, fi.absoluteFilePath());
        mFileUuid = QUuid::createUuid().toString();
        FileTransferMsg* msg = new FileTransferMsg(transferId(),  mFileUuid, fi.fileName(), mFailedItem ? mFailedItem->progressSize() : 0);
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
        if (seqNo < mTotalSeqCount) {
            QByteArray data = mFile->read(MSG_LEN);
            FilePartTransferMsg* msg = new FilePartTransferMsg(transferId(), mFileUuid, mCurrentFileIndex + 1, seqNo, data.length(), mRootProgressSize + data.length(), data);
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
        if (msg->typeId() == FileTransferHeaderInfoAckMsg::TypeID) {
            FileTransferHeaderInfoAckMsg* ackMsg = qobject_cast<FileTransferHeaderInfoAckMsg*>(msg);
            if (ackMsg->transferId() == transferId()) {
                ackMsg->deleteLater();
                sendFile();
            }
        }
        else if (msg->typeId() == FileTransferAckMsg::TypeID) {
            FileTransferAckMsg* ackMsg = qobject_cast<FileTransferAckMsg*>(msg);
            if (ackMsg->uuid() == mFileUuid) {
                emit fileSent(mConnection, ackMsg);
                mFile = new QFile(mAllFiles.at(mCurrentFileIndex).absoluteFilePath());
                qDebug() << "sending file "  << mFile->fileName();
                if (mFile->open(QFile::ReadOnly)) {
                    if (mFailedItem) {
                        mFile->seek(mFailedItem->progressSize());
                        sendFilePart(mFailedItem->seqIndex() + 1);
                        mFailedItem->deleteLater();
                        mFailedItem = 0;
                    }
                    else {
                        sendFilePart( 0);
                    }

                }
                ackMsg->deleteLater();
            }
        }
        else if (msg->typeId() == FilePartTransferAckMsg::TypeID) {
            FilePartTransferAckMsg* ackMsg = qobject_cast<FilePartTransferAckMsg*>(msg);
            if (ackMsg->uuid() == mFileUuid) {
                mCurrentSeqNo = ackMsg->seqNo();
                mRootProgressSize = ackMsg->progressSize();
                emit filePartSent(mConnection, ackMsg);
                sendFilePart(ackMsg->seqNo() + 1);
                ackMsg->deleteLater();
            }            
        }
    }
}


void FileSenderHandler::cleanup(TransferStatusFlag::ControlStatus reason)
{
    if (reason == TransferStatusFlag::Failed) {
        auto failedItem = new TransferFailedItem();
        failedItem->transferId(transferId());
        foreach(QString rootFile, mRootFiles) {
            failedItem->appendrootFiles(rootFile);
        }
        failedItem->rootFileIndex(mCurrentRootFileIndex);
        failedItem->fileIndex(mCurrentFileIndex);
        failedItem->seqIndex(mCurrentSeqNo);
        failedItem->progressSize(mRootProgressSize);
        FileMgr->addFailedTransferItem(failedItem);
    }
}
