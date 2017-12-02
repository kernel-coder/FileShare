#include "FileReceiverHandler.h"
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
#include "FileTransferUIInfoHandler.h"



FileReceiverHandler::FileReceiverHandler(Connection* conn, FileTransferMsg *msg, QObject *p)
    : FileHandlerBase(conn, p)
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
