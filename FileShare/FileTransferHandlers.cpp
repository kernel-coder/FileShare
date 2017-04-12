#include "FileTransferHandlers.h"
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

#define MSG_LEN (1*1024*1024)


FileHandler::FileHandler(Connection *conn, QObject *p)
    : QThread(p)
    , mConnection(conn)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(started()), this, SLOT(onThreadStarted()));
}


void FileHandler::onThreadStarted()
{
    connect(mConnection, SIGNAL(newMessageArrived(Connection*,Message*)), this, SLOT(onMessageComeFrom(Connection*,Message*)));
    connect(this, SIGNAL(sendMsg(Message*)), mConnection, SLOT(sendMessage(Message*)));
    handleThreadStarting();
}


void FileHandler::onMessageComeFrom(Connection *conn, Message *msg)
{
    if (conn == mConnection) {
        handleMessageComingFrom(conn, msg);
    }
}


FileSenderHandler::FileSenderHandler(Connection* conn, const QStringList& files, QObject *p)
    : FileHandler(conn, p)
    , mRootFiles(files)
    , mCurrentRootFileIndex(0)
    , mCurrentFileIndex(0)
    , mFile(0)
    , mIndexOfBasePath(-1)
    , mRootTotalSize(0)
{
}


void FileSenderHandler::handleThreadStarting()
{
    sendRootFile();
}


void FileSenderHandler::parseFile(const QFileInfo &fi)
{
    if (fi.isDir()) {
        QDir dir(fi.absoluteFilePath());
        QFileInfoList fis = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
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
        mRootUuid = QUuid::createUuid().toString();
        QFileInfo fi(mRootFiles.at(mCurrentRootFileIndex));
        mRootTotalSize = 0;
        parseFile(fi);
        mIndexOfBasePath = fi.isDir() ? fi.absoluteFilePath().lastIndexOf(QDir(fi.absoluteFilePath()).dirName()) : -1;
        mCurrentFileIndex = 0;
        FileTransferHeaderInfoMsg* msg = new FileTransferHeaderInfoMsg(mRootUuid, fi.absoluteFilePath(), mAllFiles.length(), mRootTotalSize);
        emit sendingRootFile(msg);
        emit sendMsg(msg);
        sendFile();
    }
    else {
        exit();
    }
}


void FileSenderHandler::sendFile()
{
    if (mCurrentFileIndex < mAllFiles.length()) {
        QFileInfo fi = mAllFiles.at(mCurrentFileIndex);
        emit startingFile(fi.absoluteFilePath());
        mFileUuid = QUuid::createUuid().toString();
        FileTransferMsg* msg = new FileTransferMsg(mRootUuid, mFileUuid, fi.fileName());
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
    if (seqNo < mTotalSeqCount) {
        QByteArray data = mFile->read(MSG_LEN);
        FilePartTransferMsg* msg = new FilePartTransferMsg(mRootUuid, mFileUuid, mCurrentFileIndex + 1, seqNo, data.length(), data);
        emit sendMsg(msg);
    }
    else {
        mFile->close();
        mFile->deleteLater();
        mCurrentFileIndex++;
        sendFile();
    }
}


void FileSenderHandler::handleMessageComingFrom(Connection *sender, Message *msg)
{
    if (sender == mConnection) {
        if (msg->typeId() == FileTransferAckMsg::TypeID) {
            FileTransferAckMsg* ackMsg = qobject_cast<FileTransferAckMsg*>(msg);
            if (ackMsg->uuid() == mFileUuid) {
                emit fileSent(ackMsg);
                mFile = new QFile(mAllFiles.at(mCurrentFileIndex).absoluteFilePath());
                qDebug() << "sending file "  << mFile->fileName();
                if (mFile->open(QFile::ReadOnly)) {
                    sendFilePart(0);
                }
            }
        }
        else if (msg->typeId() == FilePartTransferAckMsg::TypeID) {
            FilePartTransferAckMsg* ackMsg = qobject_cast<FilePartTransferAckMsg*>(msg);
            if (ackMsg->uuid() == mFileUuid) {
                emit filePartSent(ackMsg);
                sendFilePart(ackMsg->seqNo() + 1);
            }
        }
    }
}


FileReceiverHandler::FileReceiverHandler(Connection* conn, FileTransferMsg *msg, QObject *p)
    : FileHandler(conn, p)
    , mFileMsg(msg)
    , mFile(0)
{
}


void FileReceiverHandler::handleThreadStarting()
{
    QString filename = Utils::me()->dataDirCommon(mFileMsg->basePath());
    Utils::me()->makePath(filename);
    if (!filename.endsWith("/")) filename += "/";
    filename += mFileMsg->filename();
    mFile = new QFile(filename);
    qDebug() << "Receving file "  << filename;
    mFile->open(QFile::WriteOnly);
    FileTransferAckMsg* msg = new FileTransferAckMsg(mFileMsg->rootUuid(), mFileMsg->uuid(), mFileMsg->filename());
    msg->basePath(mFileMsg->basePath());
    msg->size(mFileMsg->size());
    msg->seqCount(mFileMsg->seqCount());
    msg->fileNo(mFileMsg->fileNo());
    emit sendMsg(msg);
}


void FileReceiverHandler::handleMessageComingFrom(Connection *sender, Message *msg)
{
    if (mConnection == sender) {
        if (msg->typeId() == FilePartTransferMsg::TypeID) {
            FilePartTransferMsg* fptm = qobject_cast<FilePartTransferMsg*>(msg);
           if (fptm->uuid() == mFileMsg->uuid()) {
               mFile->write(fptm->data());
               FilePartTransferAckMsg* ackMsg = new FilePartTransferAckMsg(fptm->rootUuid(), fptm->uuid(), fptm->fileNo(), fptm->seqNo(), fptm->size());
               emit receivedFilePart(ackMsg);
               emit sendMsg(ackMsg);
               if (fptm->seqNo() + 1 == mFileMsg->seqCount()) {
                   mFile->close();
                   mFile->deleteLater();
                   exit();
               }
           }
        }
    }
}



FileTransferUIInfoHandler* FileTransferUIInfoHandler::me()
{
    static FileTransferUIInfoHandler* _gftuih = 0;
    if (_gftuih == 0) {
        _gftuih = new FileTransferUIInfoHandler(qApp);
    }
    return _gftuih;
}


struct FileTransferUIInfoHandlerPrivate {
    QHash<QString, RootFileSentUIInfo*> mSentInfoStore;
    QHash<QString, RootFileReceivedUIInfo*> mReceivedInfoStore;
};

FileTransferUIInfoHandler::FileTransferUIInfoHandler(QObject *p)
 : QObject(p)
{
    d = new FileTransferUIInfoHandlerPrivate();
}


FileTransferUIInfoHandler::~FileTransferUIInfoHandler() {delete d;}


void FileTransferUIInfoHandler::addSenderHandler(Connection* conn, FileSenderHandler *fsh)
{
    connect(fsh, SIGNAL(sendingRootFile(FileTransferHeaderInfoMsg*)), SLOT(onSendingRootFile(FileTransferHeaderInfoMsg*)));
    connect(fsh, SIGNAL(fileSent(FileTransferAckMsg*)), SLOT(onFileSent(FileTransferAckMsg*)));
    connect(fsh, SIGNAL(filePartSent(FilePartTransferAckMsg*)), SLOT(onFilePartSent(FilePartTransferAckMsg*)));
}


void FileTransferUIInfoHandler::onSendingRootFile(FileTransferHeaderInfoMsg *msg)
{
    if (!d->mSentInfoStore.contains(msg->rootUuid())) {
        RootFileSentUIInfo* info = new RootFileSentUIInfo(this);
        info->filePath(msg->filePath());
        info->countTotalFile(msg->fileCount());
        info->countFileSent(0);
        info->sizeTotalFile(msg->totalSize());
        info->sizeFileSent(0);
        d->mSentInfoStore[msg->rootUuid()] = info;
    }
}


void FileTransferUIInfoHandler::onFileSent(FileTransferAckMsg *msg)
{

}


void FileTransferUIInfoHandler::onFilePartSent(FilePartTransferAckMsg *msg)
{
    RootFileSentUIInfo* rfi = d->mSentInfoStore.value(msg->rootUuid(), 0);
    if (rfi) {
        rfi->sizeFileSent(rfi->sizeFileSent() + msg->size());
        rfi->countFileSent(msg->fileNo() - 1);
        if (rfi->sizeFileSent() == rfi->sizeTotalFile()) {
            rfi->countFileSent(msg->fileNo());
        }
    }
}


void FileTransferUIInfoHandler::addRootFileReceiverHandler(Connection* conn, FileTransferHeaderInfoMsg *msg)
{
    if (!d->mReceivedInfoStore.contains(msg->rootUuid())) {
        RootFileReceivedUIInfo* info = new RootFileReceivedUIInfo(this);
        info->filePath(msg->filePath());
        info->countTotalFile(msg->fileCount());
        info->countFileReceived(0);
        info->sizeTotalFile(msg->totalSize());
        info->sizeFileReceived(0);
        d->mReceivedInfoStore[msg->rootUuid()] = info;
    }
}


void FileTransferUIInfoHandler::addReceiverHandler(Connection* conn, FileReceiverHandler *frh)
{
    connect(frh, SIGNAL(receivedFilePart(FilePartTransferAckMsg*)), SLOT(onReceivedFilePart(FilePartTransferAckMsg*)));
}


void FileTransferUIInfoHandler::onReceivedFilePart(FilePartTransferAckMsg *msg)
{
    RootFileReceivedUIInfo* rfi = d->mReceivedInfoStore.value(msg->rootUuid(), 0);
    if (rfi) {
        rfi->sizeFileReceived(rfi->sizeFileReceived() + msg->size());
        rfi->countFileReceived(msg->fileNo() - 1);
        if (rfi->sizeFileReceived() == rfi->sizeTotalFile()) {
            rfi->countFileReceived(msg->fileNo());
        }
    }
}
