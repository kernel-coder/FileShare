#pragma once

#include "JObject.h"
#include <QThread>
#include "Messages/TransferControlMsg.h"

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
    const int ONE_MB = (1024 * 1024);
    const int MSG_LEN = (4 * ONE_MB);
    FileHandlerBase(Connection* conn, const QString& transferId = "", QObject* p = 0);
    QString transferId() const;
    Connection* connection();
    void destroyMyself(TransferStatusFlag::ControlStatus reason);

signals:
    void sendMsg(Message* msg);
    void transferDone();

public:
    MetaPropertyPublicSet_Ex(TransferStatusFlag::ControlStatus, transferStatus)

private slots:
    void onThreadStarted();
    void onMessageComeFrom(Connection* conn, Message* msg);

protected:    
    virtual void handleThreadStarting() = 0;
    virtual void handleMessageComingFrom(Connection* conn, Message* msg) = 0;
    virtual void cleanup(TransferStatusFlag::ControlStatus){}

protected:    
    QString mTransferId;
    Connection* mConnection;

};

