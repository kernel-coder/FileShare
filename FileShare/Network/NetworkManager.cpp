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
#include "Utils.h"

#define IP_PORT_PAIR(ip, port) QString("%1:%2").arg(ip).arg(port)


NetworkManager* NetworkManager::me()
{
    static NetworkManager* _gNM = nullptr;
    if (_gNM == nullptr) {
        _gNM = new NetworkManager(qApp);
    }
    return _gNM;
}


NetworkManager::NetworkManager(QObject *parent) :
        QObject(parent)
{
    QSettings s;
    _username = s.value("username", QHostInfo::localHostName()).toString();
    _status = (PeerViewInfoMsg::PeerStatus)s.value("userstatus", (int)PeerViewInfoMsg::Free).toInt();
    _broadcastingEnabled = s.value("bcenabled", true).toBool();
    _saveFolderName = s.value("savedFolderName", Utils::me()->dataDirCommon()).toString();

    mpPeerManager = new PeerManager(this, this);
    _port = mServer.serverPort();
    mpPeerManager->setServerPort(_port);
    mpPeerManager->startBroadcasting(_broadcastingEnabled);

    connect(mpPeerManager, SIGNAL(newPeer(Connection*)), SLOT(newConnection(Connection*)));
    connect(&mServer, SIGNAL(newPeer(Connection*)), SLOT(newConnection(Connection*)));
}


NetworkManager::~NetworkManager()
{
    closeAllSocks();
}


void NetworkManager::updateBCEnabledChanged(bool on)
{
    broadcastingEnabled(on);
    QSettings s;
    s.setValue("bcenabled", on);
    mpPeerManager->startBroadcasting(on);
}


void NetworkManager::updateSavedFolderPath(const QString &path)
{
    saveFolderName(path);
    QSettings s;
    s.setValue("savedFolderName", path);
    qDebug() << "saving path set to " << path;
}

void NetworkManager::connectManual(const QString &host, int port)
{
    mpPeerManager->connectManual(QHostAddress(host), port);
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


void NetworkManager::sendMessage(Connection *conn, Message *msg)
{
    if(conn) {
        conn->sendMessage(msg);
    }
}


bool NetworkManager::addPendingPeers(const QHostAddress &senderIp, int port, Connection *conn)
{
    QString key = IP_PORT_PAIR(senderIp.toIPv4Address(), port);
    if (!mPendingPeers.contains(key)) {
        mPendingPeers.insert(key, conn);
        return true;
    }
    return false;
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


Connection* NetworkManager::createConnection()
{
    Connection* conn = new Connection(this);
    connect(conn, SIGNAL(readyForUse()), this, SLOT(onReadyForUse()));
    connect(conn, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectionError(QAbstractSocket::SocketError)));
    connect(conn, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(conn, SIGNAL(newMessageArrived(Connection*, Message*)), this, SLOT(onNewMessageArrived(Connection*, Message*)));
    return conn;
}


void NetworkManager::connectionHandShakeFialed()
{

}


void NetworkManager::newConnection(Connection *conn)
{
    //connect(conn, SIGNAL(readyForUse()), this, SLOT(onReadyForUse()));
}


void NetworkManager::onReadyForUse()
{
    qDebug() << "ReadyForUse get called";
    Connection *conn = qobject_cast<Connection *>(sender());
    QString key = IP_PORT_PAIR(conn->peerAddress().toIPv4Address(), conn->peerViewInfo()->port());    
    removePendingPeers(conn);

    if (mPeers.contains(key)) {
        removeConnection(conn);
    }

    if (!mPeers.contains(key)) {        
        mPeers.insert(key, conn);
        qDebug() << "peer connected, total peer count " << mPeers.size();
        emit newParticipant(conn);
        StatusViewer::me()->showTip(conn->peerViewInfo()->name() + tr(" has just come in the network"), LONG_DURATION);
    }
}


void NetworkManager::onDisconnected()
{
    if (Connection *conn = qobject_cast<Connection *>(sender())){
        if (mPeers.values().contains(conn)) {
            removeConnection(conn);
        }
    }
}


void NetworkManager::connectionError(QAbstractSocket::SocketError /* socketError */)
{
    if (Connection *conn = qobject_cast<Connection *>(sender())){
        if (mPeers.values().contains(conn)) {
            removeConnection(conn);
        }
    }
}


void NetworkManager::removeConnection(Connection *conn)
{
    QHashIterator<QString, Connection*> iter(mPeers);
    while (iter.hasNext()) {
        iter.next();
        if (iter.value() == conn) {
            mPeers.remove(iter.key());
            emit participantLeft(conn);
            StatusViewer::me()->showTip(conn->peerViewInfo()->name() + tr(" has just left from the network"), LONG_DURATION);
            qDebug() << "peer left, total peer count " << mPeers.size();
            break;
        }
    }

    conn->deleteLater();
}


void NetworkManager::replacePendingPeer(const QHostAddress &senderIp, int port, Connection* conn)
{
    QString key = IP_PORT_PAIR(senderIp.toIPv4Address(), port);
    auto exitingConn = mPendingPeers.value(key, NULL);
    if (exitingConn) {
        mPendingPeers.remove(key);
        exitingConn->close();
        delete exitingConn;
    }
    addPendingPeers(senderIp, port, conn);
}


void NetworkManager::onNewMessageArrived(Connection *conn, Message *msg)
{
    if (mPeers.values().contains(conn)) {
        emit newMsgCome(conn, msg);
    }
}


void NetworkManager::closeAllSocks()
{
    QList<Connection*> socks = mPeers.values();

    foreach(Connection* conn, socks){
        if(conn){
            conn->close();
            removeConnection(conn);
        }
    }
}
