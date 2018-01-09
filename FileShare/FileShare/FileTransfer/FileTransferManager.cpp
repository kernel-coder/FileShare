#include "FileTransferManager.h"
#include <QCoreApplication>
#include "NetworkManager.h"
#include "Connection.h"
#include <QDebug>
#include "Messages/ShareRequestMsg.h"
#include "Messages/ShareResponseMsg.h"
#include "Messages/FileTransferMsg.h"
#include "Messages/FilePartTransferMsg.h"
#include "Messages/ChatMsg.h"
#include "FileSenderHandler.h"
#include "FileReceiverHandler.h"
#include "HistoryManager.h"
#include "TrayManager.h"
#include "Utils.h"
#include <QQmlEngine>



RootFileUIInfo::RootFileUIInfo(QObject *p) : JObject(p)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

UITransferInfoItem::UITransferInfoItem(QObject *p) : JObject(p)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

UITransferInfoItem* UITransferInfoItem::create(RootFileUIInfo* rootFileInfo)
{
    UITransferInfoItem* item = new UITransferInfoItem();
    item->itemId(QUuid::createUuid().toString());
    rootFileInfo->setParent(item);
    item->fileInfo(rootFileInfo);
    item->isFileTransfer(true);
    return item;
}


UITransferInfoItem* UITransferInfoItem::create(const QString& chatMsg, bool sending)
{
    UITransferInfoItem* item = new UITransferInfoItem();
    item->itemId(QUuid::createUuid().toString());
    item->fileInfo(new RootFileUIInfo(item));
    item->isFileTransfer(false);
    item->chatMsg(chatMsg);
    item->isChatSending(sending);
    return item;
}


FileTransferManager* FileTransferManager::me()
{
    static FileTransferManager* _gftuih = nullptr;
    if (_gftuih == nullptr) {
        _gftuih = new FileTransferManager(qApp);
    }
    return _gftuih;
}


struct FileTransferManagerPrivate {
    QHash<QString, FileHandlerBase*> TransferHandlers;
    FileTransferManager* q;
    TransferFailedItems* FailedItems;
    FileTransferManagerPrivate(FileTransferManager* qp)
        : q(qp)
    {
        FailedItems = new TransferFailedItems(q);
        QByteArray fileData = Utils::me()->readFile(Utils::me()->machineHistoryDir(QString("failed_transfers.json")));
        if (!fileData.isEmpty()) {
            if(!FailedItems->importFromJson(fileData)) {
                qDebug() << "Failed to read failed-transfers items";
            }
        }
    }

    ~FileTransferManagerPrivate()
    {
        TransferHandlers.clear();
        saveFailedTransfer();
    }

    void saveFailedTransfer()
    {
        Utils::me()->writeFile(Utils::me()->machineHistoryDir(QString("failed_transfers.json")), FailedItems->exportToJson());
    }

    bool addTransferHandler(FileHandlerBase* handler)
    {
        if (!TransferHandlers.contains(handler->transferId())) {
            TransferHandlers[handler->transferId()] = handler;

            QObject::connect(handler, SIGNAL(transferDone()), q, SLOT(onTransferDone()));

            QObject::connect(handler, &FileHandlerBase::transferStatusChanged, [=](TransferStatusFlag::ControlStatus status) {
                auto uiInfo = HistoryMgr->getHistoryItemByTransferId(handler->connection(), handler->transferId());
                if (uiInfo) {
                    uiInfo->fileInfo()->transferStatus(status);
                }
            });

            return true;
        }
        return false;
    }
};


FileTransferManager::FileTransferManager(QObject *p)
 : QObject(p)
{
    d = new FileTransferManagerPrivate(this);
    connect(NetMgr, SIGNAL(newMsgCome(Connection*, Message*)), SLOT(onNewMsgCome(Connection*, Message*)));
}


FileTransferManager::~FileTransferManager() {delete d;}


void FileTransferManager::addFailedTransferItem(TransferFailedItem *item)
{
    for (int i = 0; i < d->FailedItems->countfailedTransfers(); i++) {
        if (d->FailedItems->itemfailedTransfersAt(i)->transferId() == item->transferId()) {
            d->FailedItems->removefailedTransfersAt(i)->deleteLater();
            break;
        }
    }

    item->setParent(d->FailedItems);
    d->FailedItems->appendfailedTransfers(item);
    d->saveFailedTransfer();
}


TransferFailedItem* FileTransferManager::removeFailedTransferItem(const QString &transferId)
{
    TransferFailedItem* item = 0;
    for (int i = 0; i < d->FailedItems->countfailedTransfers(); i++) {
        if (d->FailedItems->itemfailedTransfersAt(i)->transferId() == transferId) {
            item = d->FailedItems->removefailedTransfersAt(i);
            break;
        }
    }

    if (item) {
        d->saveFailedTransfer();
    }
    return item;
}


bool FileTransferManager::resumeFailedTransfer(Connection *conn, const QString &transferId)
{
    auto item = removeFailedTransferItem(transferId);
    if (item) {
        FileSenderHandler* handler = new FileSenderHandler(conn, item, this);
        addSenderHandler(conn, handler);
        handler->initialize();
        return true;
    }
    else {
        TrayMgr->showAppMessage("LAN Sharing", "Failed to resume", QString("No item found with transfer id %1").arg(transferId));
    }
    return false;
}


void FileTransferManager::shareFilesTo(Connection *conn, const QList<QUrl> &urls)
{
    qDebug() << urls.first().toString();
    QStringList files = Utils::me()->urlsToFiles(urls);
    foreach (QString rootFile, files) {
        FileSenderHandler* handler = new FileSenderHandler(conn, QStringList(rootFile), this);
        addSenderHandler(conn, handler);
        handler->initialize();
    }
}


void FileTransferManager::sendChatTo(Connection *conn, const QString &msg)
{
    ChatMsg* chatMsg = new ChatMsg(msg);
    conn->sendMessage(chatMsg);
    emit chatTransfer(conn, UITransferInfoItem::create(msg, true));
}


void FileTransferManager::onNewMsgCome(Connection *conn, Message *msg)
{
    if (msg->typeId() == FileTransferHeaderInfoMsg::TypeID) {
        FileReceiverHandler* handler = new FileReceiverHandler(conn, qobject_cast<FileTransferHeaderInfoMsg*>(msg));
        addReceiverHandler(conn, handler, qobject_cast<FileTransferHeaderInfoMsg*>(msg));
        handler->initialize();
    }
    else if (msg->typeId() == TransferControlMsg::TypeID) {
        auto cmsg = qobject_cast<TransferControlMsg*>(msg);
        auto handler = d->TransferHandlers.value(cmsg->transferId(), 0);
        if (!handler) {
            resumeFailedTransfer(conn, cmsg->transferId());
            msg->deleteLater();
        }
    }
    else if (msg->typeId() == ChatMsg::TypeID) {
        emit chatTransfer(conn, UITransferInfoItem::create((qobject_cast<ChatMsg*>(msg))->string(), false));
        msg->deleteLater();
    }
}


void FileTransferManager::addSenderHandler(Connection* conn, FileSenderHandler *fsh)
{
    connect(fsh, SIGNAL(sendingRootFile(Connection*, FileTransferHeaderInfoMsg*, QString)),
            SLOT(onSendingRootFile(Connection*, FileTransferHeaderInfoMsg*, QString)));
    connect(fsh, SIGNAL(transferInfoUpdated(Connection*,QString,quint64,int)),
            SLOT(onTransferInfoUpdated(Connection*,QString,quint64,int)));
    d->addTransferHandler(fsh);
}


void FileTransferManager::onSendingRootFile(Connection* conn, FileTransferHeaderInfoMsg *msg, const QString& sourcePath)
{
    UITransferInfoItem* uiInfo = HistoryMgr->getHistoryItemByTransferId(conn, msg->transferId());
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


void FileTransferManager::onTransferInfoUpdated(Connection* conn, const QString& transferId, quint64 progressSize, int fileNo)
{
    auto uiInfo = HistoryMgr->getHistoryItemByTransferId(conn, transferId);
    if (uiInfo) {
        uiInfo->fileInfo()->sizeFileProgress(progressSize);
        uiInfo->fileInfo()->countFileProgress(fileNo - 1);
        if (uiInfo->fileInfo()->sizeFileProgress() == uiInfo->fileInfo()->sizeTotalFile()) {
            uiInfo->fileInfo()->countFileProgress(fileNo);
        }
    }
}


QString FileTransferManager::saveFolderPathForTransferID(Connection* conn, const QString &transferId)
{
    auto uiInfo = HistoryMgr->getHistoryItemByTransferId(conn, transferId);
    if (uiInfo) {
        return uiInfo->fileInfo()->filePathRoot();
    }
    return NetMgr->saveFolderName();
}


void FileTransferManager::applyControlStatus(Connection* conn, RootFileUIInfo* fileInfo, int iStatus)
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


void FileTransferManager::deleteItem(Connection* conn, const QString& itemId)
{
    auto uiItem = HistoryMgr->getHistoryItemByItemId(conn, itemId);
    if (uiItem && uiItem->isFileTransfer()) {
        auto handler = d->TransferHandlers.value(uiItem->fileInfo()->transferId(), 0);
        if (handler) {
            handler->destroyMyself(TransferStatusFlag::Delete);
            TransferControlMsg* msg = new TransferControlMsg;
            msg->transferId(uiItem->fileInfo()->transferId());
            msg->status(TransferStatusFlag::Delete);
            conn->sendMessage(msg);
        }
        auto failedItem = removeFailedTransferItem(uiItem->fileInfo()->transferId());
        if (failedItem) {
            failedItem->deleteLater();
        }
    }
    HistoryMgr->removeHistoryItemByItemId(conn, itemId);    
}


void FileTransferManager::addReceiverHandler(Connection* conn, FileReceiverHandler *frh, FileTransferHeaderInfoMsg* msg)
{
    UITransferInfoItem* uiInfo = HistoryMgr->getHistoryItemByTransferId(conn, msg->transferId());
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
        connect(frh, SIGNAL(transferInfoUpdated(Connection*,QString,quint64,int)),
                SLOT(onTransferInfoUpdated(Connection*,QString,quint64,int)));
    }
}


void FileTransferManager::onTransferDone()
{
    auto handler = qobject_cast<FileHandlerBase*>(sender());
    if (d->TransferHandlers.contains(handler->transferId())) {
        d->TransferHandlers.remove(handler->transferId());
    }
}
