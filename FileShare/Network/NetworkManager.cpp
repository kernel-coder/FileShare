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
    //connect(GameSettings::me(),SIGNAL(playerNameChanged(Game::Player)),SLOT(checkPCPlayerInfoChanged(Game::Player)));
    //connect(GameSettings::me(),SIGNAL(playerColorChanged(Game::Player)),SLOT(checkPCPlayerInfoChanged(Game::Player)))
}


NetworkManager::~NetworkManager()
{
    closeAllSocks();
}


void NetworkManager::checkPCPlayerInfoChanged()
{
    QList<Connection*> socks = mPeers.values();

    foreach(Connection* pConn,socks){
        if(pConn){
            pConn->sendClientViewInfo();
        }
    }
}

bool NetworkManager::sendMessage(Connection *pConn, Message *pMsg)
{
    if(pConn){
        return pConn->sendMessage(pMsg);
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
    Connection* existingConn = hasConnection(conn->peerAddress(), conn->peerPort());
    if (existingConn == NULL) {
        connect(conn, SIGNAL(readyForUse()), this, SLOT(readyForUse()));
    }
    else {
        delete conn;
    }
}

void NetworkManager::readyForUse()
{
    Connection *conn = qobject_cast<Connection *>(sender());
    QString key = IP_PORT_PAIR(conn->peerAddress().toIPv4Address(), conn->peerPort());
    qDebug() << "new party conneccted " << key;

    if (!mPeers.contains(key)) {
        mPeers.insert(key, conn);
        connect(conn, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(connectionError(QAbstractSocket::SocketError)));
        connect(conn, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
        connect(conn, SIGNAL(newMessageArrived(Connection*,Message*)), SLOT(newMessageArrived(Connection*,Message*)));
        emit newParticipant(conn);
        StatusViewer::me()->showTip(conn->peerViewInfo()->name() + tr(" has just come in the network"), LONG_DURATION);
    }  
}


void NetworkManager::disconnectSignal(Connection *conn)
{
    disconnect(conn, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(connectionError(QAbstractSocket::SocketError)));
    disconnect(conn, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    disconnect(conn, SIGNAL(readyForUse()), this, SLOT(readyForUse()));
    disconnect(conn, SIGNAL(newMessageArrived(Connection*,Message*)),this,SLOT(newMessageArrived(Connection*,Message*)));
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
    if (Connection *pConnection = qobject_cast<Connection *>(sender())){
        removeConnection(pConnection);
    }
}

void NetworkManager::removeConnection(Connection *conn)
{
    QHashIterator<QString, Connection*> iter(mPeers);
    while (iter.hasNext()) {
        iter.next();
        if (iter.value() == conn) {
            mPeers.remove(iter.key());
            qDebug() << "removed connection " << iter.key();
            break;
        }
    }

    StatusViewer::me()->showTip(conn->peerViewInfo()->name() + tr("has just left from the network"), LONG_DURATION);
    conn->deleteLater();
}


void NetworkManager::newMessageArrived(Connection *pConn, Message *pMsg)
{
    if(/*GameSettings::me()->setting(Game::BlockOthersWhileNetPlay).value<bool>() && */
       pConn != mpPlayingWith && status() == PeerViewInfoMsg::Busy){
        ChatMsg *pMsg = new ChatMsg(tr("Soryy, I am busy now. I will chat with you later."));
        pConn->sendMessage(pMsg);

        if(pMsg->typeId() == ChatMsg::TypeID){
            mChatMsgsWhilePlaying[pConn] = dynamic_cast<ChatMsg*>(pMsg);
        }
        return;
    }

    if(pMsg->typeId() == ChatMsg::TypeID){
        emit chatMsgCame(pConn,dynamic_cast<ChatMsg*>(pMsg));
    }
    else if(pMsg->typeId() == PlayRequestMsg::TypeID){
        emit playRequestCame(pConn,dynamic_cast<PlayRequestMsg*>(pMsg));
    }
    else if(pMsg->typeId() == PlayRequestResultMsg::TypeID){
        emit playRequestResultCame(pConn,dynamic_cast<PlayRequestResultMsg*>(pMsg));
    }
    else if(pMsg->typeId() == GameCanceledMsg::TypeID){
        emit gameCanceledMsgCame(pConn,dynamic_cast<GameCanceledMsg*>(pMsg));
    }
    else if(pMsg->typeId() == LineAddedMsg::TypeID){
        emit lineAddedMsgCame(pConn,dynamic_cast<LineAddedMsg*>(pMsg));
    }

    emit newMsgCame(pConn, pMsg);
    emit newMsgCame();
}


void NetworkManager::closeAllSocks()
{
    QList<Connection*> socks = mPeers.values();

    foreach(Connection* pConn,socks){
        if(pConn){
            disconnectSignal(pConn);
            pConn->close();
        }
    }
}
