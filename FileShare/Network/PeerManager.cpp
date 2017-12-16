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


static const unsigned BroadcastPort = 45000;


PeerManager::PeerManager(NetworkManager *netMgr, QObject *parent) 
: QObject(parent)
, mpNetManager(netMgr)
{    
    mnServerPort = 0;    
    connect(&mBroadcastTimer, SIGNAL(timeout()), SLOT(sendBroadcastDatagram()));
}


PeerManager::~PeerManager()
{
    mBroadcastSocket.close();
}


void PeerManager::initialize()
{
    updateAddresses();
    mBroadcastTimer.setInterval(NetMgr->broadcastInterval());
    mBroadcastSocket.bind(QHostAddress::Any, BroadcastPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(&mBroadcastSocket, SIGNAL(readyRead()), SLOT(readBroadcastDatagram()));
}


void PeerManager::setServerPort(int port)
{
    mnServerPort = port;
}


void PeerManager::startBroadcasting(bool on)
{
    if (on) {
        if (!mBroadcastTimer.isActive()) {
            QTimer::singleShot(100, this, SLOT(sendBroadcastDatagram()));
        }
        mBroadcastTimer.start();
    }
    else {
        mBroadcastTimer.stop();
    }
}


void PeerManager::updateBCInterval(int interval)
{
    mBroadcastTimer.setInterval(interval);
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
    if (!mpNetManager->broadcastingEnabled()){
        return;
    }

    //qDebug() << "Starting BC...";
    QMutexLocker locker(&mMutex);
    QByteArray datagram = serverInfo();
    bool bValidBroadcastAddresses = true;

    int counter = 0;
    foreach (QHostAddress address, mBroadcastAddresses){
        if (mBroadcastSocket.writeDatagram(datagram, address, BroadcastPort) == -1){
            bValidBroadcastAddresses = false;
        }
        else {
            //qDebug() << "BC to " << mIPAddresses.at(counter).toString();
        }
        counter++;
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
        QByteArray datagram(srvrInfo.size(), 0);

        if (mBroadcastSocket.readDatagram(datagram.data(), datagram.size(), &senderIp, &senderPort) == -1) {
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

        ServerInfoMsg *msg = qobject_cast<ServerInfoMsg*>(MsgSystem::readAndContruct(stream));

        if (!msg || (msg->typeId() != ServerInfoMsg::TypeID) || msg->port() <= 0 || msg->port() == mnServerPort) {
            continue;
        }

        connectManual(senderIp, msg->port());
    }
}


void PeerManager::connectManual(const QHostAddress &host, int port)
{
    if (mpNetManager->hasPendingConnection(host, port) == NULL) {
        Connection *conn = mpNetManager->hasConnection(host, port);
        if (conn == NULL){
            conn = NetMgr->createConnection();
            mpNetManager->addPendingPeers(host, port, conn);
            qDebug() << "connecting to peer: " << host.toString() << port;
            connect(conn, SIGNAL(connected()), this, SLOT(onPeerConnected()));
            connect(conn, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onPeerConnectingError(QAbstractSocket::SocketError)));
            conn->connectToHost(host, port);
        }
    }
}


void PeerManager::onPeerConnected()
{
    Connection *conn = qobject_cast<Connection*>(sender());
    if(conn) {
        qDebug() << "Peer conneciton success " <<  conn->peerAddress().toString();
        disconnect(conn, SIGNAL(connected()), this, SLOT(onPeerConnected()));
        disconnect(conn, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onPeerConnectingError(QAbstractSocket::SocketError)));
        emit newPeer(conn);
    }
}

void PeerManager::onPeerConnectingError(QAbstractSocket::SocketError socketError)
{
    Connection *conn = qobject_cast<Connection*>(sender());
    if(conn) {
        qDebug() << "Peer conneciton error " <<  conn->errorString();
        disconnect(conn, SIGNAL(connected()), this, SLOT(onPeerConnected()));
        disconnect(conn, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onPeerConnectingError(QAbstractSocket::SocketError)));
        mpNetManager->removePendingPeers(conn);
        conn->deleteLater();
    }
}


void PeerManager::updateAddresses()
{
    mBroadcastAddresses.clear();
    mIPAddresses.clear();

    if (!mpNetManager->broadcastingEnabled()) return;

    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        if (interface.isValid()) {
            foreach (QNetworkAddressEntry entry, interface.addressEntries()) {
                QHostAddress broadcastAddress = entry.broadcast();
                if (broadcastAddress != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost && !broadcastAddress.isLoopback()
                        && !mBroadcastAddresses.contains(broadcastAddress) && broadcastAddress.protocol() == QAbstractSocket::IPv4Protocol) {
                    //qDebug() << "BD address found " << broadcastAddress.toString() << " /// " << entry.ip().toString();
                    mBroadcastAddresses << broadcastAddress;
                    mIPAddresses << entry.ip();
                }
            }
        }
    }
}
