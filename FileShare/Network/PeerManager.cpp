#include "PeerManager.h"
#include "Connection.h"
#include "NetworkManager.h"
#include <QProcess>
#include <QNetworkInterface>
#include "Messages/ServerInfoMsg.h"
#include "Messages/MsgSystem.h"
#include "Messages/PeerViewInfoMsg.h"
#include <QMutexLocker>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>

static const qint32 BroadcastInterval = 10000;
static const unsigned BroadcastPort = 45000;

PeerManager::PeerManager(NetworkManager *pNetManager, QObject *parent) :
    QObject(parent),mpNetManager(pNetManager)
{
    updateAddresses();
    mnServerPort = 0;

    mBroadcastSocket.bind(QHostAddress::Any, BroadcastPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(&mBroadcastSocket, SIGNAL(readyRead()),this, SLOT(readBroadcastDatagram()));

    mBroadcastTimer.setInterval(BroadcastInterval);
    connect(&mBroadcastTimer, SIGNAL(timeout()),this, SLOT(sendBroadcastDatagram()));
    sendBroadcastDatagram();
}


PeerManager::~PeerManager()
{
    mBroadcastSocket.close();
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
        if (mBroadcastSocket.writeDatagram(datagram, address, BroadcastPort) == -1){
            bValidBroadcastAddresses = false;
        }
    }

    if (!bValidBroadcastAddresses){
        updateAddresses();
    }
}


QByteArray PeerManager::serverInfo()
{
    ServerInfoMsg msg(mnServerPort);
    QByteArray block;
    QDataStream dg(&block, QIODevice::WriteOnly);
    msg.write(dg);
    return block;
}


void PeerManager::readBroadcastDatagram()
{
    QMutexLocker locker(&mMutex);
    QByteArray srvrInfo = serverInfo();

    while (mBroadcastSocket.hasPendingDatagrams()) {
        if(mBroadcastSocket.pendingDatagramSize() < srvrInfo.size()) {
            return;
        }

        QHostAddress senderIp;
        quint16 senderPort;
        QByteArray datagram;
        datagram.resize(srvrInfo.size());

        if (mBroadcastSocket.readDatagram(datagram.data(), datagram.size(),&senderIp, &senderPort) == -1) {
            continue;
        }

        if (isLocalHostAddress(senderIp)){
            continue;
        }

        if ( senderIp.isNull() || senderIp == QHostAddress::LocalHost ||
                senderIp.isLoopback() || mBroadcastAddresses.contains(senderIp)
             || mIPAddresses.contains(senderIp)) {
            continue;
        }

        QDataStream stream(datagram);

        Message *pMsg = MsgSystem::readAndContruct(stream);

        if(!pMsg || (pMsg->typeId() != ServerInfoMsg::TypeID)) {
            continue;
        }

        ServerInfoMsg *pSIMsg = qobject_cast<ServerInfoMsg*>(pMsg);

        if(!pSIMsg || pSIMsg->port() <= 0 || pSIMsg->port() == mnServerPort){
            continue;
        }
        int port = pSIMsg->port();

        qDebug() << "peer id: " << senderIp.toString() << port;

        if (mpNetManager->hasPendingConnection(senderIp, port) == NULL) {
            Connection *conn = mpNetManager->hasConnection(senderIp, port);
            if (conn == NULL){
                conn = new Connection(0, this);
                mpNetManager->addPendingPeers(senderIp, port, conn);
                qDebug() << "connecting to peer: " << senderIp.toString() << port;
                connect(conn, SIGNAL(connected()), SLOT(onPeerConnected()));
                connect(conn, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(onPeerConnectingError(QAbstractSocket::SocketError)));
                conn->connectToHost(senderIp, port);
            }
        }
    }
}


void PeerManager::onPeerConnected()
{
    Connection *conn = qobject_cast<Connection*>(sender());
    if(conn) {
        qDebug() << "Peer conneciton success " <<  conn->peerAddress().toString();
        conn->disconnect(this);
        mpNetManager->removePendingPeers(conn);
        emit newPeer(conn);
    }
}

void PeerManager::onPeerConnectingError(QAbstractSocket::SocketError socketError)
{
    Connection *conn = qobject_cast<Connection*>(sender());
    if(conn) {
        qDebug() << "Peer conneciton error " <<  conn->errorString();
        conn->disconnect(this);
        mpNetManager->removePendingPeers(conn);
        conn->deleteLater();
    }
}


void PeerManager::updateAddresses()
{
    mBroadcastAddresses.clear();
    mIPAddresses.clear();

    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        if (interface.isValid()) {
            foreach (QNetworkAddressEntry entry, interface.addressEntries()) {
                QHostAddress broadcastAddress = entry.broadcast();
                if (broadcastAddress != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost && !broadcastAddress.isLoopback()
                        && !mBroadcastAddresses.contains(broadcastAddress)) {
                    qDebug() << "BD address found " << broadcastAddress.toString() << " /// " << entry.ip().toString();
                    mBroadcastAddresses << broadcastAddress;
                    mIPAddresses << entry.ip();
                }
            }
        }
    }
}
