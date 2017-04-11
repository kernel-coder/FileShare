#include "FileTransferManager.h"
#include <QCoreApplication>
#include "NetworkManager.h"
#include "Connection.h"
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

FileSenderHandler::FileSenderHandler(Connection* conn, const QStringList& files, QObject *p)
    : JObject(p)
    , mConnection(conn)
    , mRootFiles(files)
    , mCurrentRootFileIndex(0)
    , mCurrentFileIndex(0)
    , mFile(0)
{

}

void FileSenderHandler::startSending()
{
    sendRootFile();
}


void FileSenderHandler::parseFile(const QFileInfo &fi)
{
    if (fi.isDir()) {
        QDir dir = fi.absoluteDir();
        QFileInfoList fis = dir.entryInfoList(QDir::AllDirs | QDir::NoDot | QDir::NoDotDot);
        foreach (QFileInfo fi, fis) {
            parseFile(fi);
        }
    }
    else {
        mAllFiles.append(fi);
    }
}


void FileSenderHandler::sendRootFile()
{
    mAllFiles.clear();
    if (mCurrentRootFileIndex < mRootFiles.length()) {
        QFileInfo fi(mRootFiles.at(mCurrentRootFileIndex));
        parseFile(fi);
        mCurrentBasePath = fi.isDir() ? fi.absoluteDir().dirName() : "";
        mCurrentFileIndex = 0;
        sendFile();
    }
    else {
        emit finished();
    }
}


void FileSenderHandler::sendFile()
{
    if (mCurrentFileIndex < mAllFiles.length()) {
        QFileInfo fi = mAllFiles.at(mCurrentFileIndex);
        emit startingFile(fi.absoluteFilePath());
        mCurrentUUID = QUuid::createUuid().toString();
        FileTransferMsg* msg = new FileTransferMsg(mCurrentUUID, fi.fileName());
        msg->basePath(mCurrentBasePath);
        msg->size(fi.size());
        mTotalSeqCount = qCeil(((qreal)msg->size())/MSG_LEN);
        msg->seqCount(mTotalSeqCount);
        NetMgr->sendMessage(mConnection, msg);        
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
        FilePartTransferMsg* msg = new FilePartTransferMsg(mCurrentUUID, seqNo, data);
        NetMgr->sendMessage(mConnection, msg);
    }
    else {
        mFile->close();
        mFile->deleteLater();
        sendFile();
    }
}


void FileSenderHandler::onNewMsgCame(Connection *sender, Message *msg)
{
    if (sender == mConnection) {
        if (msg->typeId() == FilePartTransferAckMsg::TypeID) {
            FilePartTransferAckMsg* ackMsg = qobject_cast<FilePartTransferAckMsg*>(msg);
            if (ackMsg->uuid() == mCurrentUUID) {
                mFile = new QFile(mAllFiles.at(mCurrentFileIndex).absoluteFilePath());
                if (mFile->open(QFile::ReadOnly)) {
                    sendFilePart(0);
                }
            }
        }
        else if (msg->typeId() == FilePartTransferAckMsg::TypeID) {
            FilePartTransferAckMsg* ackMsg = qobject_cast<FilePartTransferAckMsg*>(msg);
            if (ackMsg->uuid() == mCurrentUUID) {
                sendFilePart(ackMsg->seqNo() + 1);
            }
        }
    }
}


FileReceiverHandler::FileReceiverHandler(Connection* conn, FileTransferMsg *msg, QObject *p)
    : JObject(p)
    , mConnection(conn)
    , mFileMsg(msg)
    , mFile(0)
{

}


void FileReceiverHandler::startReceiving()
{
    QString filename = Utils::me()->dataDirCommon(mFileMsg->basePath());
    Utils::me()->makePath(filename);
    if (!filename.endsWith("/")) filename += "/";
    filename += mFileMsg->filename();
    mFile = new QFile(filename);
    mFile->open(QFile::WriteOnly);
    FileTransferAckMsg* msg = new FileTransferAckMsg(mFileMsg->uuid(), mFileMsg->filename());
    msg->basePath(mFileMsg->basePath());
    msg->size(mFileMsg->size());
    msg->seqCount(mFileMsg->seqCount());
    NetMgr->sendMessage(mConnection, msg);
}


void FileReceiverHandler::onNewMsgCame(Connection *sender, Message *msg)
{
    if (mConnection == sender) {
        if (msg->typeId() == FilePartTransferMsg::TypeID) {
            FilePartTransferMsg* fptm = qobject_cast<FilePartTransferMsg*>(msg);
           if (fptm->uuid() == mFileMsg->uuid()) {
               mFile->write(fptm->data());
               if (fptm->seqNo() + 1 == mFileMsg->seqCount()) {
                   mFile->close();
                   mFile->deleteLater();
               }
               FilePartTransferAckMsg* ackMsg = new FilePartTransferAckMsg(fptm->uuid(), fptm->seqNo());
               NetMgr->sendMessage(mConnection, ackMsg);
           }
        }
    }
}


static int gRequestIdCounter = 1;
typedef QPair<Connection*, QStringList> RequestInfo;

struct FileTransferManagerPri {
    QHash<int, RequestInfo> RequestQueue;
};

FileTransferManager* FileTransferManager::me()
{
    static FileTransferManager* _gftm = nullptr;
    if (_gftm == nullptr) {
        _gftm = new FileTransferManager(qApp);
    }
    return _gftm;
}


FileTransferManager::FileTransferManager(QObject* p)
    : JObject(p), d(new FileTransferManagerPri)
{
    connect(NetMgr, SIGNAL(newMsgCame(Connection*, Message*)), SLOT(onNewMsgCame(Connection*,Message*)));
}


void FileTransferManager::shareFilesTo(Connection *conn, const QList<QUrl> &urls)
{
    qDebug() << urls.first().toString();
    QStringList files = Utils::me()->urlsToFiles(urls);
    FileSenderHandler* handler = new FileSenderHandler(conn, files);
    QThread* thread = new QThread(this);
    handler->moveToThread(thread);
    connect(handler, SIGNAL(finished()), handler, SLOT(deleteLater()));
    connect(handler, SIGNAL(finished()), thread, SLOT(deleteLater()));
    QMetaObject::invokeMethod(handler, SLOT(startSending()));
}


void FileTransferManager::onNewMsgCame(Connection *sender, Message *msg)
{
    if (msg->typeId() == FileTransferMsg::TypeID) {

        FileReceiverHandler* handler = new FileReceiverHandler(sender, qobject_cast<FileTransferMsg*>(msg));
        QThread* thread = new QThread(this);
        handler->moveToThread(thread);
        connect(handler, SIGNAL(finished()), handler, SLOT(deleteLater()));
        connect(handler, SIGNAL(finished()), thread, SLOT(deleteLater()));
        QMetaObject::invokeMethod(handler, SLOT(startReceiving()));
    }
}
