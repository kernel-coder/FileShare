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
    QHash<QString, FileHandlerBase*> TransferHandlers;
    QHash<QString, RootFileUIInfo* >  UIInfoStore;

    bool addTransferHandler(FileHandlerBase* handler)
    {
        if (!TransferHandlers.contains(handler->transferId())) {
            TransferHandlers[handler->transferId()] = handler;

            QObject::connect(handler, &FileHandlerBase::transferDone, [=]() {
                if (TransferHandlers.contains(handler->transferId())) {
                    TransferHandlers.remove(handler->transferId());
                }
            });

            QObject::connect(handler, &FileHandlerBase::transferStatusChanged, [=](TransferStatusFlag::ControlStatus status) {
                auto uiInfo = UIInfoStore.value(handler->transferId(), 0);
                if (uiInfo) {
                    uiInfo->transferStatus(status);
                }
            });

            return true;
        }
        return false;
    }
};


FileTransferUIInfoHandler::FileTransferUIInfoHandler(QObject *p)
 : QObject(p)
{
    d = new FileTransferUIInfoHandlerPrivate();
}


FileTransferUIInfoHandler::~FileTransferUIInfoHandler() {delete d;}


void FileTransferUIInfoHandler::addSenderHandler(Connection* conn, FileSenderHandler *fsh)
{
    connect(fsh, SIGNAL(sendingRootFile(Connection*, FileTransferHeaderInfoMsg*, QString)),
            SLOT(onSendingRootFile(Connection*, FileTransferHeaderInfoMsg*, QString)));
    connect(fsh, SIGNAL(fileSent(Connection*, FileTransferAckMsg*)),
            SLOT(onFileSent(Connection*, FileTransferAckMsg*)));
    connect(fsh, SIGNAL(filePartSent(Connection*, FilePartTransferAckMsg*)),
            SLOT(onFilePartSent(Connection*, FilePartTransferAckMsg*)));
    d->addTransferHandler(fsh);
}


void FileTransferUIInfoHandler::onSendingRootFile(Connection* conn, FileTransferHeaderInfoMsg *msg, const QString& sourcePath)
{
    if (!d->UIInfoStore.contains(msg->transferId())) {
        RootFileUIInfo* info = new RootFileUIInfo(this);
        info->transferId(msg->transferId());
        info->isSending(true);
        info->filePath(msg->filePath());
        info->countTotalFile(msg->fileCount());
        info->countFileProgress(0);
        info->sizeTotalFile(msg->totalSize());
        info->sizeFileProgress(0);
        info->filePathRoot(sourcePath);
        info->transferStatus(TransferStatusFlag::Running);
        d->UIInfoStore[msg->transferId()] = info;
        emit fileTransfer(conn, UITransferInfoItem::create(conn, info));
    }
}


void FileTransferUIInfoHandler::onFileSent(Connection* conn, FileTransferAckMsg *msg)
{

}


void FileTransferUIInfoHandler::onFilePartSent(Connection* conn, FilePartTransferAckMsg *msg)
{
    RootFileUIInfo* rfi = d->UIInfoStore.value(msg->transferId(), 0);
    if (rfi) {
        rfi->sizeFileProgress(rfi->sizeFileProgress() + msg->size());
        rfi->countFileProgress(msg->fileNo() - 1);
        if (rfi->sizeFileProgress() == rfi->sizeTotalFile()) {
            rfi->countFileProgress(msg->fileNo());
        }
    }
}


QString FileTransferUIInfoHandler::saveFolderPathForTransferID(const QString &transferId)
{
    RootFileUIInfo* info = d->UIInfoStore.value(transferId, 0);
    if (info) {
        return info->filePathRoot();
    }
    return NetMgr->saveFolderName();
}


void FileTransferUIInfoHandler::applyControlStatus(Connection* conn, RootFileUIInfo* fileInfo, int istatus)
{
    TransferStatusFlag::ControlStatus status = (TransferStatusFlag::ControlStatus)istatus;
    auto hanlder = d->TransferHandlers.value(fileInfo->transferId(), 0);
    if (hanlder) {
        hanlder->transferStatus(status);
    }

    TransferControlMsg* msg = new TransferControlMsg;
    msg->transferId(fileInfo->transferId());
    msg->status(status);
    conn->sendMessage(msg);
}


void FileTransferUIInfoHandler::addReceiverHandler(Connection* conn, FileReceiverHandler *frh, FileTransferHeaderInfoMsg* msg)
{
    if (!d->UIInfoStore.contains(msg->transferId())) {
        RootFileUIInfo* info = new RootFileUIInfo(this);
        info->transferId(msg->transferId());
        info->isSending(false);
        info->filePath(msg->filePath());
        info->countTotalFile(msg->fileCount());
        info->countFileProgress(0);
        info->sizeTotalFile(msg->totalSize());
        info->sizeFileProgress(0);
        info->filePathRoot(NetMgr->saveFolderName());
        info->transferStatus(TransferStatusFlag::Running);
        d->UIInfoStore[msg->transferId()] = info;
        emit fileTransfer(conn, UITransferInfoItem::create(conn, info));
    }


    if (d->addTransferHandler(frh)) {
        connect(frh, SIGNAL(receivedFilePart(Connection*, FilePartTransferAckMsg*)), SLOT(onReceivedFilePart(Connection*, FilePartTransferAckMsg*)));
    }
}


void FileTransferUIInfoHandler::onReceivedFilePart(Connection* conn, FilePartTransferAckMsg *msg)
{
    RootFileUIInfo* rfi = d->UIInfoStore.value(msg->transferId(), 0);
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
