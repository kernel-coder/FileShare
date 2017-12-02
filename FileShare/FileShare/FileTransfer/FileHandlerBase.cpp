#include "FileHandlerBase.h"
#include "Connection.h"
#include "Messages/Message.h"


FileHandlerBase::FileHandlerBase(Connection *conn, QObject *p)
    : QThread(p)
    , mConnection(conn)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(started()), this, SLOT(onThreadStarted()));
}


void FileHandlerBase::onThreadStarted()
{
    connect(mConnection, SIGNAL(newMessageArrived(Connection*,Message*)), this, SLOT(onMessageComeFrom(Connection*,Message*)));
    connect(this, SIGNAL(sendMsg(Message*)), mConnection, SLOT(sendMessage(Message*)));
    handleThreadStarting();
}


void FileHandlerBase::onMessageComeFrom(Connection *conn, Message *msg)
{
    if (conn == mConnection) {
        handleMessageComingFrom(conn, msg);
    }
}

