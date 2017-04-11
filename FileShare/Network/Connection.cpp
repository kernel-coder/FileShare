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
}

void TcpSocket::setupSocket(int sockId)
{
    connect(this, SIGNAL(readyRead()), SLOT(dataReadyToRead()));

    if (sockId > 0 ) {
        if(setSocketDescriptor(sockId)) {
            sendClientViewInfo();
        }
    }
    else {
        connect(this, SIGNAL(connected()), SLOT(sendClientViewInfo()));
    }
}


void TcpSocket::sendClientViewInfo()
{
    PeerViewInfoMsg* pvi = new PeerViewInfoMsg(NetMgr->username(), NetMgr->port(), NetMgr->status());
    sendMessage(pvi);
}


void TcpSocket::onDataReadReady()
{
    QMutexLocker locker(&mMutex);
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_6);

    while(bytesAvailable() >= (int)sizeof(quint16)){
        if (mnBlockSize == 0){
            in >> mnBlockSize;
        }

        if (bytesAvailable() < mnBlockSize){
            return;
        }

        mnBlockSize = 0;

        Message *msg = MsgSystem::readAndContruct(in);
        emit newMessageCome(msg);
    }
}


void TcpSocket::sendMessage(Message *msg)
{
    if(msg){
        qDebug() << QString("Message Sending: %1, %2").arg(msg->typeId()).arg(msg->metaObject()->className());
        QByteArray block;
        QDataStream stream(&block, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_4_6);
        stream << (quint64)0;
        msg->write(stream);
        stream.device()->seek(0);
        stream << (quint64)(block.size() - sizeof(quint64));
        this->write(block);
        msg->deleteLater();
    }
}



Connection::Connection(int sockId, QObject *parent)
    : QObject(parent)
    , _peerViewInfo(0)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);        
    QThread* thread = new QThread(this);
    mSocket = new TcpSocket();
    mSocket->moveToThread(thread);
    connect(mSocket, SIGNAL(destroyed(QObject*)), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), mSocket, SLOT(deleteLater()));
    thread->start();

    connect(mSocket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(mSocket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(error(QAbstractSocket::SocketError)));
    connect(mSocket, SIGNAL(newMessageCome(Message*)), this, SLOT(onMessageReceived(Message*)));
    connect(this, SIGNAL(fireSendMessage(Message*)), mSocket, SLOT(sendMessage(Message*)));

    connect(this, SIGNAL(sigConnectToHost(QHostAddress,quint16)), mSocket, SLOT(slotConnectToHost(QHostAddress,quint16)), Qt::DirectConnection);
    connect(this, SIGNAL(sigDisconnectFromHost()), mSocket, SLOT(slotDisconnectFromHost()), Qt::DirectConnection);
    connect(this, SIGNAL(sigClose()), mSocket, SLOT(slotClose()), Qt::DirectConnection);
}


void Connection::sendClientViewInfo()
{
    PeerViewInfoMsg* pvi = new PeerViewInfoMsg(NetMgr->username(), NetMgr->port(), NetMgr->status());
    emit fireSendMessage(pvi);
}


void Connection::sendMessage(Message *msg)
{
    emit fireSendMessage(msg);
}


void Connection::onMessageReceived(Message* msg)
{
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


