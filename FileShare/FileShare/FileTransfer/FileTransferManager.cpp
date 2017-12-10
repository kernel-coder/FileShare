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
#include "FileTransferUIInfoHandler.h"
#include "FileSenderHandler.h"
#include "FileReceiverHandler.h"
#include "Utils.h"


struct FileTransferManagerPri {
    FileTransferManager* q;
    TransferFailedItems* FailedItems;
    FileTransferManagerPri(FileTransferManager* qp)
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

    ~FileTransferManagerPri()
    {
        saveFailedTransfer();
    }

    void saveFailedTransfer()
    {
        Utils::me()->writeFile(Utils::me()->machineHistoryDir(QString("failed_transfers.json")), FailedItems->exportToJson());
    }
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
    : JObject(p)
{
    d = new FileTransferManagerPri(this);
    connect(NetMgr, SIGNAL(newMsgCome(Connection*, Message*)), SLOT(onNewMsgCome(Connection*, Message*)));
}


FileTransferManager::~FileTransferManager()
{
    delete d;
}


void FileTransferManager::addFailedTransferItem(TransferFailedItem *item)
{
    for (int i = 0; i < d->FailedItems->countTransferFailedItem(); i++) {
        if (d->FailedItems->itemTransferFailedItemAt(i)->transferId() == item->transferId()) {
            d->FailedItems->removeTransferFailedItemAt(i)->deleteLater();
            break;
        }
    }

    item->setParent(d->FailedItems);
    d->FailedItems->appendTransferFailedItem(item);
    d->saveFailedTransfer();
}


TransferFailedItem* FileTransferManager::removeFailedTransferItem(const QString &transferId)
{
    TransferFailedItem* item = 0;
    for (int i = 0; i < d->FailedItems->countTransferFailedItem(); i++) {
        if (d->FailedItems->itemTransferFailedItemAt(i)->transferId() == transferId) {
            item = d->FailedItems->removeTransferFailedItemAt(i);
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
        FileMgrUIHandler->addSenderHandler(conn, handler);
        handler->start();
        return true;
    }
    return false;
}


void FileTransferManager::shareFilesTo(Connection *conn, const QList<QUrl> &urls)
{
    qDebug() << urls.first().toString();
    QStringList files = Utils::me()->urlsToFiles(urls);
    foreach (QString rootFile, files) {
        FileSenderHandler* handler = new FileSenderHandler(conn, QStringList(rootFile), this);
        FileMgrUIHandler->addSenderHandler(conn, handler);
        handler->start();
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
        FileMgrUIHandler->addReceiverHandler(conn, handler, qobject_cast<FileTransferHeaderInfoMsg*>(msg));
        handler->start();
    }
    else if (msg->typeId() == TransferControlMsg::TypeID) {
        auto cmsg = qobject_cast<TransferControlMsg*>(msg);
        auto handler = FileMgrUIHandler->getHandler(cmsg->transferId());
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
