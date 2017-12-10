#include "FileHandlerBase.h"
#include "Connection.h"
#include "NetworkManager.h"
#include "Messages/Message.h"
#include <QUuid>


FileHandlerBase::FileHandlerBase(Connection *conn, const QString& transferId, QObject *p)
    : QThread(p)
    , mConnection(conn)
    , _transferStatus(TransferStatusFlag::NotStarted)
{
    mTransferId = transferId.isEmpty() ? QUuid::createUuid().toString() : transferId;
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(started()), this, SLOT(onThreadStarted()));
    connect(NetMgr, &NetworkManager::participantLeft, [=](Connection* conn) {
        if (conn == mConnection) {
            destroyMyself(false);
        }
    });
}


QString FileHandlerBase::transferId() const
{
    return mTransferId;
}


Connection* FileHandlerBase::connection()
{
    return mConnection;
}

void FileHandlerBase::destroyMyself(bool success)
{
    cleanup(success);
    transferStatus(success ? TransferStatusFlag::Finished : TransferStatusFlag::Failed);
    emit transferDone();
    exit();
    this->deleteLater();
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
        if (msg->typeId() == TransferControlMsg::TypeID) {
            TransferControlMsg* tcmsg = qobject_cast<TransferControlMsg*>(msg);
            if (tcmsg->transferId() == mTransferId) {
                transferStatus(tcmsg->status());
            }
        }
        else {
            handleMessageComingFrom(conn, msg);
        }
    }
}

