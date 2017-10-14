#include "HistoryManager.h"
#include <QCoreApplication>
#include "FileTransferHandlers.h"
#include "FileTransferManager.h"
#include "Connection.h"
#include "Utils.h"
#include <QHash>

class HistoryManagerPrivate {
    HistoryManager* q_ptr;
public:
    static HistoryManager* _ParentStatic;
    QHash<QString, QList<UITransferInfoItem*>> HistoryMap;

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
}


void HistoryManager::onNewTransferArrived(Connection *conn, UITransferInfoItem *item)
{
    d_ptr->HistoryMap[conn->peerViewInfo()->deviceId()].append(item);
}


QList<UITransferInfoItem*> HistoryManager::getHistoryForDevice(const QString &deviceId)
{
    return d_ptr->HistoryMap.value(deviceId, QList<UITransferInfoItem*>());
}
