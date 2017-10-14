#pragma once

#include "JObject.h"
#include "FileTransferHandlers.h"

#define HistoryMgr HistoryManager::me()

class HistoryManagerPrivate;
class Connection;

class MachineHistoryItem : public JObject
{
    Q_OBJECT
public:
    MachineHistoryItem(QObject* p = 0) : JObject(p) {}
    MetaPropertyPublicSet(QString, deviceId)
    MetaPropertyPublicSet_Ptr_List(UITransferInfoItem,  histories)
};

class HistoryManager : public QObject
{
    Q_OBJECT    
public:
    explicit HistoryManager(QObject *parent = 0);
    static HistoryManager* me();

    Q_INVOKABLE QVariantList getHistoryForDevice(const QString& deviceId);
signals:

public slots:

private slots:
    void onNewTransferArrived(Connection* conn, UITransferInfoItem* item);
    void onConnectionClosed(Connection* conn);

private:
    HistoryManagerPrivate* d_ptr;
};
