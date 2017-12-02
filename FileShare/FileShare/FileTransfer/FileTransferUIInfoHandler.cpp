#include "FileTransferUIInfoHandler.h"
#include "FileHandlerBase.h"
#include "FileSenderHandler.h"
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
        emit fileTransfer(conn, UITransferInfoItem::create(conn, info));
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
        emit fileTransfer(conn, UITransferInfoItem::create(conn, info));
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


UITransferInfoItem* UITransferInfoItem::create(QObject* parent, RootFileUIInfo* rootFileInfo)
{
    UITransferInfoItem* item = new UITransferInfoItem(parent);
    item->fileInfo(rootFileInfo);
    item->isFileTransfer(true);
    return item;
}


UITransferInfoItem* UITransferInfoItem::create(QObject* parent, const QString& chatMsg, bool sending)
{
    UITransferInfoItem* item = new UITransferInfoItem(parent);
    item->fileInfo(new RootFileUIInfo(parent));
    item->isFileTransfer(false);
    item->chatMsg(chatMsg);
    item->isChatSending(sending);
    return item;
}
