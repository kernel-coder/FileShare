#include "HistoryManager.h"
#include <QCoreApplication>
#include "FileTransfer/FileTransferManager.h"
#include "NetworkManager.h"
#include "Connection.h"
#include "Utils.h"
#include <QHash>
#include <QDebug>

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
    d_ptr = new HistoryManagerPrivate(this);

    connect(FileMgr, SIGNAL(chatTransfer(Connection*,UITransferInfoItem*)),
            SLOT(onNewTransferArrived(Connection*,UITransferInfoItem*)));
    connect(FileMgrUIHandler, SIGNAL(fileTransfer(Connection*,UITransferInfoItem*)),
            SLOT(onNewTransferArrived(Connection*,UITransferInfoItem*)));
    connect(NetMgr, SIGNAL(participantLeft(Connection*)), SLOT(onConnectionClosed(Connection*)));
}


void HistoryManager::onNewTransferArrived(Connection *conn, UITransferInfoItem *item)
{
    QString deviceId = conn->peerViewInfo()->deviceId();
    MachineHistoryItem* mhi = d_ptr->HistoryMap.value(deviceId, nullptr);
    if (mhi != nullptr) {
        item->setParent(mhi);
        mhi->appendUITransferInfoItem(item);
    }
}


QVariantList HistoryManager::getHistoryForDevice(const QString &deviceId)
{
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
            for (int i = 0; i < mhi->countUITransferInfoItem(); i++) {
                auto item = mhi->itemUITransferInfoItemAt(i);
                if (item->isFileTransfer() && item->fileInfo()->sizeFileProgress() < item->fileInfo()->sizeTotalFile()) {
                    item->fileInfo()->transferStatus(TransferStatusFlag::Pause);
                }
            }
        }
    }

    return mhi->histories();
}


UITransferInfoItem* HistoryManager::getHistoryItem(const QString &deviceId, const QString& transferId)
{
    MachineHistoryItem* mhi = d_ptr->HistoryMap.value(deviceId, nullptr);
    if (mhi == nullptr) {
        return 0;
    }
    else {
        for (int i = 0; i < mhi->countUITransferInfoItem(); i++) {
            auto item = mhi->itemUITransferInfoItemAt(i);
            if (item->isFileTransfer() && item->fileInfo()->transferId() == transferId) {
                return item;
            }
        }
    }
    return 0;
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
