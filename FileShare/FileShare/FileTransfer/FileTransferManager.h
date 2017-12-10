#pragma once

#include "JObject.h"
#include <QUrl>
#include <QFileInfoList>
#include <QThread>

#define FileMgr FileTransferManager::me()


class Connection;
class Message;

struct FileTransferManagerPri;
class UITransferInfoItem;

class TransferFailedItem : public JObject {
    Q_OBJECT
public:
    Q_INVOKABLE TransferFailedItem(QObject* p = 0) : JObject(p) {}
    MetaPropertyPublicSet(QString, transferId)
    MetaPropertyPublicSet_List(QString, rootFiles)
    MetaPropertyPublicSet(int, rootFileIndex)
    MetaPropertyPublicSet(int, fileIndex)
    MetaPropertyPublicSet(int, seqIndex)
    MetaPropertyPublicSet(quint64, progressSize)
};

class TransferFailedItems : public JObject {
    Q_OBJECT
public:
    TransferFailedItems(QObject* p = 0) : JObject(p) {}
    MetaPropertyPublicSet_Ptr_List(TransferFailedItem,  failedTransfers)
};

class FileTransferManager : public JObject
{
    Q_OBJECT
    FileTransferManager(QObject* p = 0);
public:
    ~FileTransferManager();
    static FileTransferManager* me();

    bool resumeFailedTransfer(Connection *conn, const QString& transferId);
    void addFailedTransferItem(TransferFailedItem* item);
    TransferFailedItem* removeFailedTransferItem(const QString& transferId);

signals:
    void chatTransfer(Connection* conn, UITransferInfoItem* uiInfo);

public slots:
    void shareFilesTo(Connection* conn, const QList<QUrl> &urls);
    void sendChatTo(Connection* conn, const QString& msg);

private slots:
    void onNewMsgCome(Connection *sender, Message *msg);

private:
    FileTransferManagerPri* d;
};
