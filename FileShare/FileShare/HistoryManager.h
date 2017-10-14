#pragma once

#include <QObject>

#define HistoryMgr HistoryManager::me()

class HistoryManagerPrivate;
class UITransferInfoItem;
class Connection;

class HistoryManager : public QObject
{
    Q_OBJECT    
public:
    explicit HistoryManager(QObject *parent = 0);
    static HistoryManager* me();

    Q_INVOKABLE QList<UITransferInfoItem*> getHistoryForDevice(const QString& deviceId);
signals:

public slots:

private slots:
    void onNewTransferArrived(Connection* conn, UITransferInfoItem* item);

private:
    HistoryManagerPrivate* d_ptr;
};
