#pragma once

#include "JObject.h"
#include <QUrl>

#define FileMgr FileTransferManager::me()

class Connection;
class Message;

struct FileTransferManagerPri;

class FileTransferManager : public JObject
{
    Q_OBJECT
    FileTransferManager(QObject* p = 0);
public:
    static FileTransferManager* me();

public slots:
    void shareFilesTo(Connection* conn, const QList<QUrl> &urls);

private slots:
    void onNewMsgCame(Connection *sender, Message *msg);

private:
    FileTransferManagerPri* d;
};
