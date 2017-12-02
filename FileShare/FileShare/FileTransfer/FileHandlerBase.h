#pragma once

#include "JObject.h"
#include <QThread>


class Connection;
class Message;
class FileTransferHeaderInfoMsg;
class FileTransferMsg;
class FileTransferAckMsg;
class FilePartTransferAckMsg;
class QFile;

class FileHandlerBase : public QThread
{
    Q_OBJECT
public:
    const int MSG_LEN = 4*1024*1024;
    FileHandlerBase(Connection* conn, QObject* p = 0);
signals:
    void sendMsg(Message* msg);

private slots:
    void onThreadStarted();
    void onMessageComeFrom(Connection* conn, Message* msg);

protected:
    virtual void handleThreadStarting() = 0;
    virtual void handleMessageComingFrom(Connection* conn, Message* msg) = 0;

protected:
    Connection* mConnection;
};

