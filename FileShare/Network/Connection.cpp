#include "Connection.h"
#include "Messages/Message.h"
#include "Messages/PeerViewInfoMsg.h"
#include "Messages/MsgSystem.h"
#include "NetworkManager.h"
#include <QMutexLocker>
#include <QSysInfo>
#include <QtQml>
#include <QTimer>
#include "Utils.h"


static int cRefCounter = 0;
Connection::Connection(QObject *parent)
    : QTcpSocket(parent)
    , _peerViewInfo(0)
    , mPeerInfoSent(false)
    , mMsgSize(0)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    _id = ++cRefCounter;
    qDebug() << "Conn Construction " << _id;
    connect(this, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(this, SIGNAL(readyRead()), this, SLOT(onDataReadReady()));
}


Connection::~Connection()
{
    qDebug() << "Conn Destruction " << _id;
}


void Connection::onConnected()
{
    qDebug() << "Connected";
    QTimer::singleShot(100, this, SLOT(sendClientViewInfo()));
}

void Connection::onDataReadReady()
{
    qDebug() << "GOT BYTES " << bytesAvailable();
    QMutexLocker locker(&mMutex);
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_6);

    while(bytesAvailable() >= (int)sizeof(qint64)) {
        if (mMsgSize == 0){
            in >> mMsgSize;
        }

        if (bytesAvailable() < mMsgSize){
            return;
        }

        mMsgSize = 0;
        Message *msg = MsgSystem::readAndContruct(in);
        qDebug() << QString("Message %1 receiving from %2").arg(msg->metaObject()->className()).arg(_id);

        if(msg){
            if(msg->typeId() == PeerViewInfoMsg::TypeID) {
                PeerViewInfoMsg* pvi = qobject_cast<PeerViewInfoMsg*>(msg);
                if(pvi != NULL) {
                    if(_peerViewInfo) {
                        _peerViewInfo->name(pvi->name());
                        _peerViewInfo->status(pvi->status());
                        pvi->deleteLater();
                    }
                    else {
                        if (!mPeerInfoSent) {
                            sendClientViewInfo();
                        }
                        peerViewInfo(pvi);
                        qDebug() << "Firing readyForuse fired " << _id;
                        emit readyForUse();
                    }
                }
            }
            else{
                emit newMessageArrived(this, msg);
            }
        }
    }
}


void Connection::sendClientViewInfo()
{
    mPeerInfoSent = true;
    PeerViewInfoMsg* pvi = new PeerViewInfoMsg(NetMgr->username(), NetMgr->port(),
                                               NetMgr->status(), Utils::me()->deviceId());
    sendMessage(pvi);
}


void Connection::sendMessage(Message *msg)
{
    if(msg){
        qDebug() << QString("Message %1 sending to %2").arg(msg->metaObject()->className()).arg(_id);
        QByteArray block;
        QDataStream stream(&block, QIODevice::ReadWrite);
        stream.setVersion(QDataStream::Qt_4_6);
        stream << (qint64)0;
        msg->write(stream);
        stream.device()->seek(0);
        stream << (qint64)(block.size() - sizeof(qint64));
        write(block);
        msg->deleteLater();
    }
}

