#include "FileTransferUIInfoHandler.h"
#include "FileHandlerBase.h"
#include "FileSenderHandler.h"
#include "FileReceiverHandler.h"
#include "NetworkManager.h"
#include "Connection.h"
#include "FileTransferManager.h"
#include <QCoreApplication>
#include <QDebug>
#include "Messages/ShareRequestMsg.h"
#include "Messages/ShareResponseMsg.h"
#include "Messages/FileTransferMsg.h"
#include "Messages/FilePartTransferMsg.h"
#include "HistoryManager.h"
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

    void addUIInfo(RootFileUIInfo* fileInfo)
    {
        if (!UIInfoStore.contains(fileInfo->transferId())) {
            UIInfoStore[fileInfo->transferId()] = fileInfo;
            QObject::connect(fileInfo, &RootFileUIInfo::destroyed, [=](){
                UIInfoStore.remove(fileInfo->transferId());
            });
        }
    }
};


FileTransferUIInfoHandler::FileTransferUIInfoHandler(QObject *p)
 : QObject(p)
{
    d = new FileTransferUIInfoHandlerPrivate();
}


FileTransferUIInfoHandler::~FileTransferUIInfoHandler() {delete d;}


FileHandlerBase* FileTransferUIInfoHandler::getHandler(const QString& transferId)
{
    return d->TransferHandlers.value(transferId, 0);
}


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
        RootFileUIInfo* info = 0;
        auto utii = HistoryMgr->getHistoryItem(conn->peerViewInfo()->deviceId(), msg->transferId());
        if (utii) {
            info = utii->fileInfo();
        }
        else {
            info = new RootFileUIInfo();
            emit fileTransfer(conn, UITransferInfoItem::create(info));
        }
        info->transferId(msg->transferId());
        info->isSending(true);
        info->filePath(msg->filePath());
        info->countTotalFile(msg->fileCount());
        info->countFileProgress(msg->fileIndex());
        info->sizeTotalFile(msg->totalSize());
        info->sizeFileProgress(msg->progressSize());
        info->filePathRoot(sourcePath);
        info->transferStatus(TransferStatusFlag::Running);
        d->addUIInfo(info);
    }
}


void FileTransferUIInfoHandler::onFileSent(Connection* conn, FileTransferAckMsg *msg)
{

}


void FileTransferUIInfoHandler::onFilePartSent(Connection* conn, FilePartTransferAckMsg *msg)
{
    RootFileUIInfo* rfi = d->UIInfoStore.value(msg->transferId(), 0);
    if (rfi) {
        rfi->sizeFileProgress(msg->progressSize());
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


void FileTransferUIInfoHandler::applyControlStatus(Connection* conn, RootFileUIInfo* fileInfo, int iStatus)
{
    TransferStatusFlag::ControlStatus status = (TransferStatusFlag::ControlStatus)iStatus;
    auto hanlder = d->TransferHandlers.value(fileInfo->transferId(), 0);    
    if (hanlder) {
        // paused transfer
        hanlder->transferStatus(status);
        TransferControlMsg* msg = new TransferControlMsg;
        msg->transferId(fileInfo->transferId());
        msg->status(status);
        conn->sendMessage(msg);
    }
    else {
        // failed transfer
        if (fileInfo->isSending()) {
            FileMgr->resumeFailedTransfer(conn, fileInfo->transferId());
        }
        else {
            TransferControlMsg* msg = new TransferControlMsg;
            msg->transferId(fileInfo->transferId());
            msg->status(status);
            conn->sendMessage(msg);
        }
    }
}


void FileTransferUIInfoHandler::addReceiverHandler(Connection* conn, FileReceiverHandler *frh, FileTransferHeaderInfoMsg* msg)
{
    if (!d->UIInfoStore.contains(msg->transferId())) {
        RootFileUIInfo* info = 0;
        auto utii = HistoryMgr->getHistoryItem(conn->peerViewInfo()->deviceId(), msg->transferId());
        if (utii) {
            info = utii->fileInfo();
        }
        else {
            info = new RootFileUIInfo();
            emit fileTransfer(conn, UITransferInfoItem::create(info));
        }
        info->transferId(msg->transferId());
        info->isSending(false);
        info->filePath(msg->filePath());
        info->countTotalFile(msg->fileCount());
        info->countFileProgress(msg->fileIndex());
        info->sizeTotalFile(msg->totalSize());
        info->sizeFileProgress(msg->progressSize());
        info->filePathRoot(NetMgr->saveFolderName());
        info->transferStatus(TransferStatusFlag::Running);
        d->addUIInfo(info);
    }


    if (d->addTransferHandler(frh)) {
        connect(frh, SIGNAL(receivedFilePart(Connection*, FilePartTransferAckMsg*)), SLOT(onReceivedFilePart(Connection*, FilePartTransferAckMsg*)));
    }
}


void FileTransferUIInfoHandler::onReceivedFilePart(Connection* conn, FilePartTransferAckMsg *msg)
{
    RootFileUIInfo* rfi = d->UIInfoStore.value(msg->transferId(), 0);
    if (rfi) {
        rfi->sizeFileProgress(msg->progressSize());
        rfi->countFileProgress(msg->fileNo() - 1);
        if (rfi->sizeFileProgress() == rfi->sizeTotalFile()) {
            rfi->countFileProgress(msg->fileNo());
        }
    }
}


UITransferInfoItem* UITransferInfoItem::create(RootFileUIInfo* rootFileInfo)
{
    UITransferInfoItem* item = new UITransferInfoItem();
    rootFileInfo->setParent(item);
    item->fileInfo(rootFileInfo);
    item->isFileTransfer(true);
    return item;
}


UITransferInfoItem* UITransferInfoItem::create(const QString& chatMsg, bool sending)
{
    UITransferInfoItem* item = new UITransferInfoItem();
    item->fileInfo(new RootFileUIInfo(item));
    item->isFileTransfer(false);
    item->chatMsg(chatMsg);
    item->isChatSending(sending);
    return item;
}
