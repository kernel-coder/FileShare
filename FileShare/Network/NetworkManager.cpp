#include "NetworkManager.h"
#include "Connection.h"
#include "PeerManager.h"
#include <QHostInfo>
#include "Messages/ChatMsg.h"
#include "Messages/PlayRequestMsg.h"
#include "Messages/PlayRequestResultMsg.h"
#include "Messages/GameCanceledMsg.h"
#include "Messages/LineAddedMsg.h"
#include "StatusViewer.h"
#include "Messages/PeerViewInfoMsg.h"
#include <QCoreApplication>
#include <QSettings>
#include <QSettings>

#define IP_PORT_PAIR(ip, port) QString("%1:%2").arg(ip).arg(port)


NetworkManager* NetworkManager::me()
{
    static NetworkManager* _nm = 0;
    if (_nm == 0) {
        _nm = new NetworkManager(qApp);
    }
    return _nm;
}


NetworkManager::NetworkManager(QObject *parent) :
        QObject(parent)
{
    QSettings s;
    _username = s.value("username", QHostInfo::localHostName()).toString();
    _status = (PeerViewInfoMsg::PeerStatus)s.value("userstatus", (int)PeerViewInfoMsg::Free).toInt();

    mpPeerManager = new PeerManager(this, this);
    _port = mServer.serverPort();
    mpPeerManager->setServerPort(_port);
    mpPeerManager->startBroadcasting();

    connect(mpPeerManager, SIGNAL(newPeer(Connection*)), SLOT(newConnection(Connection*)));
    connect(&mServer, SIGNAL(newPeer(Connection*)), SLOT(newConnection(Connection*)));
}


NetworkManager::~NetworkManager()
{
    closeAllSocks();
}


void NetworkManager::broadcastUserInfoChanged()
{
    QSettings s;
    s.setValue("username", _username);
    s.setValue("userstatus", (int)_status);

    QList<Connection*> socks = mPeers.values();

    foreach(Connection* conn,socks) {
        if(conn) {
            conn->sendClientViewInfo();
        }
    }
}


bool NetworkManager::sendMessage(Connection *conn, Message *msg)
{
    if(conn) {
        return conn->sendMessage(msg);
    }

    return false;
}


void NetworkManager::addPendingPeers(const QHostAddress &senderIp, int port, Connection *conn)
{
    QString key = IP_PORT_PAIR(senderIp.toIPv4Address(), port);
    if (!mPendingPeers.contains(key)) {
        mPendingPeers.insert(key, conn);
    }
}


void NetworkManager::removePendingPeers(Connection *conn)
{
    QHashIterator<QString, Connection*> iter(mPendingPeers);
    while (iter.hasNext()) {
        iter.next();
        if (iter.value() == conn) {
            mPendingPeers.remove(iter.key());
            qDebug() << "removed pending peer " << iter.key();
            break;
        }
    }
}


Connection *NetworkManager::hasPendingConnection(const QHostAddress &senderIp, int port)
{
    return mPendingPeers.value(IP_PORT_PAIR(senderIp.toIPv4Address(), port), NULL);
}


Connection *NetworkManager::hasConnection(const QHostAddress &senderIp, int port)
{
    return mPeers.value(IP_PORT_PAIR(senderIp.toIPv4Address(), port), NULL);
}


void NetworkManager::newConnection(Connection *conn)
{
    connect(conn, SIGNAL(readyForUse()), this, SLOT(onReadyForUse()));
}


void NetworkManager::onReadyForUse()
{
    Connection *conn = qobject_cast<Connection *>(sender());
    QString key = IP_PORT_PAIR(conn->peerAddress().toIPv4Address(), conn->peerViewInfo()->port());    
    removePendingPeers(conn);

    if (!mPeers.contains(key)) {
        qDebug() << "new connection found " << key;
        mPeers.insert(key, conn);
        connect(conn, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectionError(QAbstractSocket::SocketError)));
        connect(conn, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
        connect(conn, SIGNAL(newMessageArrived(Connection*, Message*)), this, SLOT(onNewMessageArrived(Connection*, Message*)));
        emit newParticipant(conn);
        StatusViewer::me()->showTip(conn->peerViewInfo()->name() + tr(" has just come in the network"), LONG_DURATION);
    }
    else {
        removeConnection(conn);
    }
}


void NetworkManager::onDisconnected()
{
    if (Connection *conn = qobject_cast<Connection *>(sender())){
        participantLeft(conn);
        removeConnection(conn);
    }
}


void NetworkManager::connectionError(QAbstractSocket::SocketError /* socketError */)
{
    if (Connection *conn = qobject_cast<Connection *>(sender())){
        removeConnection(conn);
    }
}


void NetworkManager::removeConnection(Connection *conn)
{
    QHashIterator<QString, Connection*> iter(mPeers);
    while (iter.hasNext()) {
        iter.next();
        if (iter.value() == conn) {
            mPeers.remove(iter.key());
            StatusViewer::me()->showTip(conn->peerViewInfo()->name() + tr("has just left from the network"), LONG_DURATION);
            qDebug() << "removed connection " << iter.key();
            break;
        }
    }

    conn->deleteLater();
}


void NetworkManager::onNewMessageArrived(Connection *conn, Message *msg)
{
    emit newMsgCame(conn, msg);
}


void NetworkManager::closeAllSocks()
{
    QList<Connection*> socks = mPeers.values();

    foreach(Connection* conn, socks){
        if(conn){
            conn->close();
            conn->deleteLater();
        }
    }
}
