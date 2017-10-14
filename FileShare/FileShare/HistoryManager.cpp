#include "HistoryManager.h"
#include <QCoreApplication>
#include "FileTransferHandlers.h"
#include "FileTransferManager.h"
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
        mhi->appendUITransferInfoItem(item);
    }
}


QList<UITransferInfoItem*> HistoryManager::getHistoryForDevice(const QString &deviceId)
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
    }

    QList<UITransferInfoItem*> items;

    for (int i = 0; i < mhi->countUITransferInfoItem(); i++) {
        items.append(mhi->itemUITransferInfoItemAt(i));
    }

    return items;
}


void HistoryManager::onConnectionClosed(Connection *conn)
{
    QString deviceId = conn->peerViewInfo()->deviceId();
    MachineHistoryItem* mhi = d_ptr->HistoryMap.value(deviceId, nullptr);
    if (mhi != nullptr) {
        Utils::me()->writeFile(Utils::me()->machineHistoryDir(QString("%1.json").arg(deviceId)),
                         mhi->exportToJson());
    }
}
