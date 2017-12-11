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
                auto uiInfo = HistoryMgr->getHistoryItem(handler->connection(), handler->transferId());
                if (uiInfo) {
                    uiInfo->fileInfo()->transferStatus(status);
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
    UITransferInfoItem* uiInfo = HistoryMgr->getHistoryItem(conn, msg->transferId());
    RootFileUIInfo* info = 0;
    if (uiInfo) {
        info = uiInfo->fileInfo();
    }
    else {
        info = new RootFileUIInfo();
        uiInfo = UITransferInfoItem::create(info);
        emit fileTransfer(conn, uiInfo);
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
}


void FileTransferUIInfoHandler::onFileSent(Connection* conn, FileTransferAckMsg *msg)
{

}


void FileTransferUIInfoHandler::onFilePartSent(Connection* conn, FilePartTransferAckMsg *msg)
{
    auto uiInfo = HistoryMgr->getHistoryItem(conn, msg->transferId());
    if (uiInfo) {
        uiInfo->fileInfo()->sizeFileProgress(msg->progressSize());
        uiInfo->fileInfo()->countFileProgress(msg->fileNo() - 1);
        if (uiInfo->fileInfo()->sizeFileProgress() == uiInfo->fileInfo()->sizeTotalFile()) {
            uiInfo->fileInfo()->countFileProgress(msg->fileNo());
        }
    }
}


QString FileTransferUIInfoHandler::saveFolderPathForTransferID(Connection* conn, const QString &transferId)
{
    auto uiInfo = HistoryMgr->getHistoryItem(conn, transferId);
    if (uiInfo) {
        return uiInfo->fileInfo()->filePathRoot();
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


void FileTransferUIInfoHandler::deleteItem(Connection* conn, RootFileUIInfo* fileInfo)
{
    HistoryMgr->removeHistoryItem(conn, fileInfo->transferId());

    auto handler = d->TransferHandlers.value(fileInfo->transferId(), 0);
    if (handler) {
        handler->destroyMyself(TransferStatusFlag::Delete);

        TransferControlMsg* msg = new TransferControlMsg;
        msg->transferId(fileInfo->transferId());
        msg->status(TransferStatusFlag::Delete);
        conn->sendMessage(msg);
    }
}


void FileTransferUIInfoHandler::addReceiverHandler(Connection* conn, FileReceiverHandler *frh, FileTransferHeaderInfoMsg* msg)
{
    UITransferInfoItem* uiInfo = HistoryMgr->getHistoryItem(conn, msg->transferId());
    RootFileUIInfo* info = 0;
    if (uiInfo) {
        info = uiInfo->fileInfo();
    }
    else {
        info = new RootFileUIInfo();
        uiInfo = UITransferInfoItem::create(info);
        emit fileTransfer(conn, uiInfo);
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


    if (d->addTransferHandler(frh)) {
        connect(frh, SIGNAL(receivedFilePart(Connection*, FilePartTransferAckMsg*)), SLOT(onReceivedFilePart(Connection*, FilePartTransferAckMsg*)));
    }
}


void FileTransferUIInfoHandler::onReceivedFilePart(Connection* conn, FilePartTransferAckMsg *msg)
{
    auto uiInfo = HistoryMgr->getHistoryItem(conn, msg->transferId());
    if (uiInfo) {
        uiInfo->fileInfo()->sizeFileProgress(msg->progressSize());
        uiInfo->fileInfo()->countFileProgress(msg->fileNo() - 1);
        if (uiInfo->fileInfo()->sizeFileProgress() == uiInfo->fileInfo()->sizeTotalFile()) {
            uiInfo->fileInfo()->countFileProgress(msg->fileNo());
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
