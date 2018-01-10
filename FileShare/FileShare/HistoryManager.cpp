#include "HistoryManager.h"
#include <QCoreApplication>
#include "FileTransfer/FileTransferManager.h"
#include "NetworkManager.h"
#include "Connection.h"
#include "Utils.h"
#include <QHash>
#include <QDebug>
#include <QQmlEngine>

class HistoryManagerPrivate {
    HistoryManager* q_ptr;
public:
    static HistoryManager* _ParentStatic;
    QHash<QString, MachineHistoryItem*> HistoryMap;

    HistoryManagerPrivate(HistoryManager* qptr)
        : q_ptr(qptr)
    {

    }
};

HistoryManager* HistoryManagerPrivate::_ParentStatic = nullptr;

HistoryManager* HistoryManager::me()
{
    if (HistoryManagerPrivate::_ParentStatic == nullptr) {
        HistoryManagerPrivate::_ParentStatic = new HistoryManager(qApp);
    }
    return HistoryManagerPrivate::_ParentStatic;
}


HistoryManager::HistoryManager(QObject *parent) : QObject(parent)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    d_ptr = new HistoryManagerPrivate(this);

    connect(FileMgr, SIGNAL(chatTransfer(Connection*,UITransferInfoItem*)),
            SLOT(onNewTransferArrived(Connection*,UITransferInfoItem*)));
    connect(FileMgr, SIGNAL(fileTransfer(Connection*,UITransferInfoItem*)),
            SLOT(onNewTransferArrived(Connection*,UITransferInfoItem*)));
    connect(NetMgr, SIGNAL(participantLeft(Connection*)), SLOT(onConnectionClosed(Connection*)));
}

HistoryManager::~HistoryManager()
{
    delete d_ptr;
    d_ptr = nullptr;
}


void HistoryManager::onNewTransferArrived(Connection *conn, UITransferInfoItem *item)
{
    QString deviceId = conn->peerViewInfo()->deviceId();
    MachineHistoryItem* mhi = d_ptr->HistoryMap.value(deviceId, nullptr);
    if (mhi != nullptr) {
        item->setParent(mhi);
        mhi->appendhistories(item);
    }
}


void HistoryManager::onConnectionClosed(Connection *conn)
{
    QString deviceId = conn->peerViewInfo()->deviceId();
    MachineHistoryItem* mhi = d_ptr->HistoryMap.value(deviceId, nullptr);
    if (mhi != nullptr) {
        Utils::me()->writeFile(Utils::me()->machineHistoryDir(QString("%1.json").arg(deviceId)), mhi->exportToJson());
        d_ptr->HistoryMap.remove(deviceId);
        mhi->deleteLater();
    }
}


QVariantList HistoryManager::getHistoryForDevice(Connection* conn)
{
    QString deviceId = conn->peerViewInfo()->deviceId();
    MachineHistoryItem* mhi = d_ptr->HistoryMap.value(deviceId, nullptr);
    if (mhi == nullptr) {
        mhi = new MachineHistoryItem(this);
        mhi->deviceId(deviceId);
        d_ptr->HistoryMap[deviceId] = mhi;
    }

    QByteArray fileData = Utils::me()->readFile(Utils::me()->machineHistoryDir(QString("%1.json").arg(deviceId)));

    if (!fileData.isEmpty()) {
        if(!mhi->importFromJson(fileData)) {
            qDebug() << "Failed to read history file for device " << deviceId;
        }
        else {
            for (int i = 0; i < mhi->counthistories(); i++) {
                auto item = mhi->itemhistoriesAt(i);
                if (item->isFileTransfer() && item->fileInfo()->sizeFileProgress() < item->fileInfo()->sizeTotalFile()) {
                    item->fileInfo()->transferStatus(TransferStatusFlag::Pause);
                }
            }
        }
    }

    return mhi->histories();
}


UITransferInfoItem* HistoryManager::getHistoryItemByTransferId(Connection* conn, const QString& transferId)
{
    MachineHistoryItem* mhi = d_ptr->HistoryMap.value(conn->peerViewInfo()->deviceId(), nullptr);
    if (mhi) {
        for (int i = 0; i < mhi->counthistories(); i++) {
            auto item = mhi->itemhistoriesAt(i);
            if (item->isFileTransfer() && item->fileInfo()->transferId() == transferId) {
                return item;
            }
        }
    }
    return nullptr;
}


bool HistoryManager::removeHistoryItemByTransferId(Connection* conn, const QString& transferId)
{
    MachineHistoryItem* mhi = d_ptr->HistoryMap.value(conn->peerViewInfo()->deviceId(), nullptr);
    if (mhi) {
        for (int i = 0; i < mhi->counthistories(); i++) {
            auto item = mhi->itemhistoriesAt(i);
            if (item->isFileTransfer() && item->fileInfo()->transferId() == transferId) {
                mhi->removehistoriesAt(i);
                emit historyItemRemoved(conn, item);
                item->deleteLater();
                return true;
            }
        }
    }
    return false;
}


UITransferInfoItem* HistoryManager::getHistoryItemByItemId(Connection* conn, const QString& itemId)
{
    MachineHistoryItem* mhi = d_ptr->HistoryMap.value(conn->peerViewInfo()->deviceId(), nullptr);
    if (mhi) {
        for (int i = 0; i < mhi->counthistories(); i++) {
            auto item = mhi->itemhistoriesAt(i);
            if (item->itemId() == itemId) {
                return item;
            }
        }
    }
    return nullptr;
}


bool HistoryManager::removeHistoryItemByItemId(Connection* conn, const QString& itemId)
{
    MachineHistoryItem* mhi = d_ptr->HistoryMap.value(conn->peerViewInfo()->deviceId(), nullptr);
    if (mhi) {
        for (int i = 0; i < mhi->counthistories(); i++) {
            auto item = mhi->itemhistoriesAt(i);
            if (item->itemId() == itemId) {
                mhi->removehistoriesAt(i);
                emit historyItemRemoved(conn, item);
                item->deleteLater();
                return true;
            }
        }
    }
    return false;
}
