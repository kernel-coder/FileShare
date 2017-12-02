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

class FileTransferManager : public JObject
{
    Q_OBJECT
    FileTransferManager(QObject* p = 0);
public:
    static FileTransferManager* me();

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
