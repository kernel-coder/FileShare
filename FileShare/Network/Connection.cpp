#include "Connection.h"
#include "Messages/Message.h"
#include "Messages/PeerViewInfoMsg.h"
#include "Messages/MsgSystem.h"
#include "NetworkManager.h"
#include <QMutexLocker>
#include <QSysInfo>
#include <QtQml>
#include <QTimer>
#include <QThread>


TcpSocket::TcpSocket(QObject * p) : QTcpSocket(p), mnBlockSize(0)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(onDataReadReady()));
}


void TcpSocket::onDataReadReady()
{
    QMutexLocker locker(&mMutex);
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_6);

    while(bytesAvailable() >= (int)sizeof(qint64)){
        if (mnBlockSize == 0){
            in >> mnBlockSize;
        }

        if (bytesAvailable() < mnBlockSize){
            return;
        }

        emit newRawMsgArrived(read(mnBlockSize));
        mnBlockSize = 0;
    }
}


void TcpSocket::sendRawMessage(const QByteArray& data)
{
    write(data);
}



Connection::Connection(int sockId, QObject *parent)
    : QThread(parent)
    , mSockId(sockId)
    , _peerViewInfo(0)
    , mSocket(0)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


Connection::~Connection()
{
    if (mSocket) {
        mSocket->deleteLater();
    }
}


void Connection::startAndWait()
{
    mMutex.lock();
    start();
    mWaitCondition.wait(&mMutex);
    mMutex.unlock();
}


void Connection::sendClientViewInfo()
{
    PeerViewInfoMsg* pvi = new PeerViewInfoMsg(NetMgr->username(), NetMgr->port(), NetMgr->status());
    sendMessage(pvi);
}


void Connection::sendMessage(Message *msg)
{
    if(msg){
        qDebug() << QString("Message Sending: %1, %2").arg(msg->typeId()).arg(msg->metaObject()->className());
        QByteArray block;
        {QDataStream stream(&block, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_4_6);
        stream << (qint64)0;
        msg->write(stream);
        stream.device()->seek(0);
        stream << (qint64)(block.size() - sizeof(qint64));}
        emit fireSendRawMessage(block);
    }
}


void Connection::onNewRawMessageReceived(const QByteArray& data)
{
    QDataStream in(data);
    in.setVersion(QDataStream::Qt_4_6);
    Message *msg = MsgSystem::readAndContruct(in);
    qDebug() << QString("Message Received: %1, %2").arg(msg->typeId()).arg(msg->metaObject()->className());

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
                    peerViewInfo(pvi);
                    qDebug() << "Firing readyForuse fired";
                    emit readyForUse();
                }
            }
        }
        else{
            emit newMessageArrived(this, msg);
        }
    }
}


void Connection::run()
{
    mSocket = new TcpSocket;

    connect(mSocket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(mSocket, SIGNAL(connected()), this, SLOT(sendClientViewInfo()));
    connect(mSocket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(error(QAbstractSocket::SocketError)));
    connect(mSocket, SIGNAL(newRawMsgArrived(QByteArray)), this, SLOT(onNewRawMessageReceived(QByteArray)));
    connect(this, SIGNAL(fireSendRawMessage(QByteArray)), mSocket, SLOT(sendRawMessage(QByteArray)));

    connect(this, SIGNAL(sigConnectToHost(QHostAddress,quint16)), mSocket, SLOT(slotConnectToHost(QHostAddress,quint16)));
    connect(this, SIGNAL(sigDisconnectFromHost()), mSocket, SLOT(slotDisconnectFromHost()));
    connect(this, SIGNAL(sigClose()), mSocket, SLOT(slotClose()));
    connect(this, SIGNAL(destroyed(QObject*)), mSocket, SLOT(deleteLater()));
    mWaitCondition.wakeAll();
    if (mSockId > 0) {
        mSocket->setSocketDescriptor(mSockId);
    }
    exec();
    mSocket->deleteLater();
}


