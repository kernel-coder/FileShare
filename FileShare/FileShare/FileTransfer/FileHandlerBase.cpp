#include "FileHandlerBase.h"
#include "Connection.h"
#include "NetworkManager.h"
#include "Messages/Message.h"
#include "HistoryManager.h"
#include <QUuid>


FileHandlerBase::FileHandlerBase(Connection *conn, const QString& transferId, QObject *p)
    : QObject(p)
    , mConnection(conn)
    , _transferStatus(TransferStatusFlag::NotStarted)
{
    mTransferId = transferId.isEmpty() ? QUuid::createUuid().toString() : transferId;
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(NetMgr, &NetworkManager::participantLeft, [=](Connection* conn) {
        if (conn == mConnection) {
            destroyMyself(TransferStatusFlag::Failed);
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

void FileHandlerBase::destroyMyself(TransferStatusFlag::ControlStatus reason)
{
    transferStatus(reason);
    cleanup(reason);
    emit transferDone();
    this->deleteLater();
}


void FileHandlerBase::initialize()
{    
    connect(mConnection, SIGNAL(newMessageArrived(Connection*,Message*)), this, SLOT(onMessageComeFrom(Connection*,Message*)));
    connect(this, SIGNAL(sendMsg(Message*)), mConnection, SLOT(sendMessage(Message*)));
    handleInitialize();
}


void FileHandlerBase::onMessageComeFrom(Connection *conn, Message *msg)
{
    if (conn == mConnection) {
        if (msg->typeId() == TransferControlMsg::TypeID) {
            TransferControlMsg* tcmsg = qobject_cast<TransferControlMsg*>(msg);
            if (tcmsg->transferId() == mTransferId) {
                transferStatus(tcmsg->status());
                if (tcmsg->status() == TransferStatusFlag::Delete) {
                    destroyMyself(TransferStatusFlag::Delete);
                    HistoryMgr->removeHistoryItemByTransferId(conn, tcmsg->transferId());
                }
            }
        }
        else {
            handleMessageComingFrom(conn, msg);
        }
    }
}

