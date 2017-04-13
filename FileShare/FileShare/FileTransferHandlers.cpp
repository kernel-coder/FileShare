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
        mRootUuid = QUuid::createUuid().toString();
        QFileInfo fi(mRootFiles.at(mCurrentRootFileIndex));
        mRootTotalSize = 0;
        parseFile(fi);
        mIndexOfBasePath = fi.isDir() ? fi.absoluteFilePath().lastIndexOf(QDir(fi.absoluteFilePath()).dirName()) : -1;
        mCurrentFileIndex = 0;
        FileTransferHeaderInfoMsg* msg = new FileTransferHeaderInfoMsg(mRootUuid, fi.fileName(), mAllFiles.length(), mRootTotalSize);
        emit sendingRootFile(mConnection, msg, fi.absolutePath());
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
        emit startingFile(mConnection, fi.absoluteFilePath());
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


FileReceiverHandler::FileReceiverHandler(Connection* conn, FileTransferMsg *msg, QObject *p)
    : FileHandler(conn, p)
    , mFileMsg(msg)
    , mFile(0)
{
}


FileReceiverHandler::~FileReceiverHandler()
{
    delete mFileMsg;
}


void FileReceiverHandler::handleThreadStarting()
{
    QString filename = FileMgrUIHandler->saveFolderPathForRootUUID(mFileMsg->rootUuid());
    filename = filename.replace("\\", "/");
    if (!filename.endsWith("/")) filename += "/";
    filename += mFileMsg->basePath();
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
               emit receivedFilePart(mConnection, ackMsg);
               emit sendMsg(ackMsg);
               if (fptm->seqNo() + 1 == mFileMsg->seqCount()) {
                   mFile->close();
                   mFile->deleteLater();
                   exit();
               }
           }
           msg->deleteLater();
        }
    }
}



FileTransferUIInfoHandler* FileTransferUIInfoHandler::me()
{
    static FileTransferUIInfoHandler* _gftuih = nullptr;
    if (_gftuih == nullptr) {
        _gftuih = new FileTransferUIInfoHandler(qApp);
    }
    return _gftuih;
}


struct FileTransferUIInfoHandlerPrivate {
    QHash < QString, RootFileUIInfo* >  mSentInfoStore;
    QHash<QString, RootFileUIInfo* >  mReceivedInfoStore;
};

FileTransferUIInfoHandler::FileTransferUIInfoHandler(QObject *p)
 : QObject(p)
{
    d = new FileTransferUIInfoHandlerPrivate();
}


FileTransferUIInfoHandler::~FileTransferUIInfoHandler() {delete d;}


void FileTransferUIInfoHandler::addSenderHandler(Connection* conn, FileSenderHandler *fsh)
{
    connect(fsh, SIGNAL(sendingRootFile(Connection*, FileTransferHeaderInfoMsg*, QString)), SLOT(onSendingRootFile(Connection*, FileTransferHeaderInfoMsg*, QString)));
    connect(fsh, SIGNAL(fileSent(Connection*, FileTransferAckMsg*)), SLOT(onFileSent(Connection*, FileTransferAckMsg*)));
    connect(fsh, SIGNAL(filePartSent(Connection*, FilePartTransferAckMsg*)), SLOT(onFilePartSent(Connection*, FilePartTransferAckMsg*)));
}


void FileTransferUIInfoHandler::onSendingRootFile(Connection* conn, FileTransferHeaderInfoMsg *msg, const QString& sourcePath)
{
    if (!d->mSentInfoStore.contains(msg->rootUuid())) {
        RootFileUIInfo* info = new RootFileUIInfo(this);
        info->isSending(true);
        info->filePath(msg->filePath());
        info->countTotalFile(msg->fileCount());
        info->countFileProgress(0);
        info->sizeTotalFile(msg->totalSize());
        info->sizeFileProgress(0);
        info->filePathRoot(sourcePath);
        d->mSentInfoStore[msg->rootUuid()] = info;
        emit fileTransfer(conn, info);
    }
}


void FileTransferUIInfoHandler::onFileSent(Connection* conn, FileTransferAckMsg *msg)
{

}


void FileTransferUIInfoHandler::onFilePartSent(Connection* conn, FilePartTransferAckMsg *msg)
{
    RootFileUIInfo* rfi = d->mSentInfoStore.value(msg->rootUuid(), 0);
    if (rfi) {
        rfi->sizeFileProgress(rfi->sizeFileProgress() + msg->size());
        rfi->countFileProgress(msg->fileNo() - 1);
        if (rfi->sizeFileProgress() == rfi->sizeTotalFile()) {
            rfi->countFileProgress(msg->fileNo());
        }
    }
}


QString FileTransferUIInfoHandler::saveFolderPathForRootUUID(const QString &rootUuid)
{
    RootFileUIInfo* info = d->mReceivedInfoStore.value(rootUuid, 0);
    if (info) {
        return info->filePathRoot();
    }
    return NetMgr->saveFolderName();
}


void FileTransferUIInfoHandler::addRootFileReceiverHandler(Connection* conn, FileTransferHeaderInfoMsg *msg)
{
    if (!d->mReceivedInfoStore.contains(msg->rootUuid())) {
        RootFileUIInfo* info = new RootFileUIInfo(this);
        info->isSending(false);

        info->filePath(msg->filePath());
        info->countTotalFile(msg->fileCount());
        info->countFileProgress(0);
        info->sizeTotalFile(msg->totalSize());
        info->sizeFileProgress(0);
        info->filePathRoot(NetMgr->saveFolderName());
        d->mReceivedInfoStore[msg->rootUuid()] = info;
        emit fileTransfer(conn, info);
    }
    msg->deleteLater();
}


void FileTransferUIInfoHandler::addReceiverHandler(Connection* conn, FileReceiverHandler *frh)
{
    connect(frh, SIGNAL(receivedFilePart(Connection*, FilePartTransferAckMsg*)), SLOT(onReceivedFilePart(Connection*, FilePartTransferAckMsg*)));
}


void FileTransferUIInfoHandler::onReceivedFilePart(Connection* conn, FilePartTransferAckMsg *msg)
{
    RootFileUIInfo* rfi = d->mReceivedInfoStore.value(msg->rootUuid(), 0);
    if (rfi) {
        rfi->sizeFileProgress(rfi->sizeFileProgress() + msg->size());
        rfi->countFileProgress(msg->fileNo() - 1);
        if (rfi->sizeFileProgress() == rfi->sizeTotalFile()) {
            rfi->countFileProgress(msg->fileNo());
        }
    }
}
