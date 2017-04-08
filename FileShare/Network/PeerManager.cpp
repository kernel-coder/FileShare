#include "PeerManager.h"
#include "Connection.h"
#include "NetworkManager.h"
#include <QProcess>
#include <QNetworkInterface>
#include "Messages/ServerInfoMsg.h"
#include "Messages/MsgSystem.h"
#include <QMutexLocker>

static const qint32 BroadcastInterval = 30000;
static const unsigned BroadcastPort = 45000;

PeerManager::PeerManager(NetworkManager *pNetManager, QObject *parent) :
    QObject(parent),mpNetManager(pNetManager)
{
    updateAddresses();
    mnServerPort = 0;

    mBroadcastSocket.bind(QHostAddress::Any, BroadcastPort, QUdpSocket::ShareAddress
                         | QUdpSocket::ReuseAddressHint);
    connect(&mBroadcastSocket, SIGNAL(readyRead()),this, SLOT(readBroadcastDatagram()));

    mBroadcastTimer.setInterval(BroadcastInterval);
    connect(&mBroadcastTimer, SIGNAL(timeout()),this, SLOT(sendBroadcastDatagram()));
}

void PeerManager::setServerPort(int nPort)
{
    mnServerPort = nPort;
}

void PeerManager::startBroadcasting()
{
    mBroadcastTimer.start();
}

bool PeerManager::isLocalHostAddress(const QHostAddress &address)
{
    foreach (QHostAddress localAddress, mIPAddresses){
        if (address == localAddress){
            return true;
        }
    }

    return false;
}

void PeerManager::sendBroadcastDatagram()
{
    QMutexLocker locker(&mMutex);

    QByteArray datagram = serverInfo();

    bool bValidBroadcastAddresses = true;

    foreach (QHostAddress address, mBroadcastAddresses){
        if (mBroadcastSocket.writeDatagram(datagram, address,BroadcastPort) == -1){
            bValidBroadcastAddresses = false;
        }
    }

    if (!bValidBroadcastAddresses){
        updateAddresses();
    }
}

QByteArray PeerManager::serverInfo()
{
    ServerInfoMsg msg(mnServerPort,mpNetManager->status());
    QByteArray block;
    QDataStream dg(&block, QIODevice::WriteOnly);
    msg.write(dg);

    return block;
}

void PeerManager::readBroadcastDatagram()
{
    QMutexLocker locker(&mMutex);

    QByteArray srvrInfo = serverInfo();

    while (mBroadcastSocket.hasPendingDatagrams()){        
        if(mBroadcastSocket.pendingDatagramSize() < srvrInfo.size()){
            return;
        }

        QHostAddress senderIp;
        quint16 senderPort;
        QByteArray datagram;
        datagram.resize(srvrInfo.size());

        if (mBroadcastSocket.readDatagram(datagram.data(), datagram.size(),&senderIp, &senderPort) == -1){
            continue;
        }

        QDataStream stream(datagram);

        Message *pMsg = MsgSystem::readAndContruct(stream);

        if(!pMsg || (pMsg->typeId() != ServerInfoMsg::TypeID)){
            continue;
        }

        ServerInfoMsg *pSIMsg = dynamic_cast<ServerInfoMsg*>(pMsg);

        if(!pSIMsg){
            continue;
        }

        int nSenderServerPort = pSIMsg->port();

        if (isLocalHostAddress(senderIp) && nSenderServerPort == mnServerPort){
            continue;
        }

        Connection *pConn = mpNetManager->hasConnection(senderIp);

        if (pConn == NULL){
            Connection *pConnection = new Connection(pSIMsg->status(), this);
            connect(pConnection,SIGNAL(connected()),SLOT(connected()));
            pConnection->connectToHost(senderIp,nSenderServerPort);
        }
        else{
            pConn->setStatus(pSIMsg->status());
        }
    }
}

void PeerManager::connected()
{
    Connection *pConn = qobject_cast<Connection*>(sender());

    if(pConn){
        emit newPeer(pConn);
        //disconnect(pConn,SIGNAL(connected()),this,SLOT(connected()));
    }
}

void PeerManager::updateAddresses()
{
    mBroadcastAddresses.clear();
    mIPAddresses.clear();

    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()){
        foreach (QNetworkAddressEntry entry, interface.addressEntries()){
            QHostAddress broadcastAddress = entry.broadcast();

            if (broadcastAddress != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost){
                mBroadcastAddresses << broadcastAddress;
                mIPAddresses << entry.ip();
            }
        }
    }
}
