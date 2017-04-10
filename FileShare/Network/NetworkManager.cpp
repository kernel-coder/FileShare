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

    QHostInfo::lookupHost(_username, this, SLOT(onLocalHostLookupDone(QHostInfo)));

    mpPeerManager = new PeerManager(this, this);
    _port = mServer.serverPort();
    mpPeerManager->setServerPort(_port);
    mpPeerManager->startBroadcasting();

    connect(mpPeerManager, SIGNAL(newPeer(Connection*)),SLOT(newConnection(Connection*)));
    connect(&mServer, SIGNAL(newPeer(Connection*)),SLOT(newConnection(Connection*)));
    //connect(GameSettings::me(),SIGNAL(playerNameChanged(Game::Player)),SLOT(checkPCPlayerInfoChanged(Game::Player)));
    //connect(GameSettings::me(),SIGNAL(playerColorChanged(Game::Player)),SLOT(checkPCPlayerInfoChanged(Game::Player)))
}


NetworkManager::~NetworkManager()
{
    closeAllSocks();
}


void NetworkManager::onLocalHostLookupDone(const QHostInfo &host)
{
    foreach (QHostAddress ha, host.addresses()) {
        if (ha.isNull() || ha.isLoopback() || ha.isMulticast() ||
                ha.protocol() != QAbstractSocket::IPv4Protocol) {
            continue;
        }
        qDebug() << "loup " << ha.toString();
    }
}

void NetworkManager::checkPCPlayerInfoChanged(/*Game::Player player*/)
{
    //if(player == Game::Player1){
        QList<Connection*> socks = mPeers.values();

        foreach(Connection* pConn,socks){
            if(pConn){
                pConn->sendClientViewInfo();
            }
        }
    //}
}

bool NetworkManager::sendMessage(Connection *pConn, Message *pMsg)
{
    if(pConn){
        return pConn->sendMessage(pMsg);
    }

    return false;
}

void NetworkManager::addPendingPeers(const QHostAddress &senderIp, Connection *conn)
{
    mPendingPeers.insert(senderIp, conn);
}

void NetworkManager::removePendingPeers(Connection *conn)
{
    foreach (Connection *pConnection, mPendingPeers.values()){
        if (conn == pConnection){
            mPendingPeers.remove(conn->peerAddress());
            break;
        }
    }
}

Connection *NetworkManager::hasPendingConnection(const QHostAddress &senderIp, int nSenderPort)
{
    if (nSenderPort == -1){
        return mPendingPeers.value(senderIp,NULL);
    }

    if (!mPendingPeers.contains(senderIp)){
        return NULL;
    }

    QList<Connection *> pConnections = mPendingPeers.values(senderIp);

    foreach (Connection *pConnection, pConnections){
        if (pConnection->peerPort() == nSenderPort){
            return pConnection;
        }
    }

    return NULL;
}

Connection *NetworkManager::hasConnection(const QHostAddress &senderIp, int nSenderPort)
{
    if (nSenderPort == -1){
        return mPeers.value(senderIp,NULL);
    }

    if (!mPeers.contains(senderIp)){
        return NULL;
    }

    QList<Connection *> pConnections = mPeers.values(senderIp);

    foreach (Connection *pConnection, pConnections){
        if (pConnection->peerPort() == nSenderPort){
            return pConnection;
        }
    }

    return NULL;
}

void NetworkManager::newConnection(Connection *pConnection)
{
    connect(pConnection, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(connectionError(QAbstractSocket::SocketError)));
    connect(pConnection, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(pConnection, SIGNAL(readyForUse()), this, SLOT(readyForUse()));
}

void NetworkManager::readyForUse()
{
    Connection *pConnection = qobject_cast<Connection *>(sender());

    if (!pConnection || (NULL != hasConnection(pConnection->peerAddress(), pConnection->peerPort()))){
        return;
    }

    connect(pConnection, SIGNAL(newMessageArrived(Connection*,Message*)),this,SLOT(newMessageArrived(Connection*,Message*)));

    mPeers.insert(pConnection->peerAddress(), pConnection);
    emit newParticipant(pConnection);
    StatusViewer::me()->showTip(pConnection->peerViewInfo()->name() + tr(" has just come in the network"), LONG_DURATION);
}


void NetworkManager::disconnectSignal(Connection *pConnection)
{
    disconnect(pConnection, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(connectionError(QAbstractSocket::SocketError)));
    disconnect(pConnection, SIGNAL(disconnected()), this, SLOT(disconnected()));
    disconnect(pConnection, SIGNAL(readyForUse()), this, SLOT(readyForUse()));
    disconnect(pConnection, SIGNAL(newMessageArrived(Connection*,Message*)),this,SLOT(newMessageArrived(Connection*,Message*)));
}

void NetworkManager::disconnected()
{
    if (Connection *pConnection = qobject_cast<Connection *>(sender())){
        removeConnection(pConnection);
    }
}

void NetworkManager::connectionError(QAbstractSocket::SocketError /* socketError */)
{
    if (Connection *pConnection = qobject_cast<Connection *>(sender())){
        removeConnection(pConnection);
    }
}

void NetworkManager::removeConnection(Connection *pConnection)
{
    if (mPeers.contains(pConnection->peerAddress())){
        mPeers.remove(pConnection->peerAddress());
        emit participantLeft(pConnection);
    }

    pConnection->deleteLater();

    StatusViewer::me()->showTip(pConnection->peerViewInfo()->name() + tr("has just left from the network"), LONG_DURATION);
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


void NetworkManager::setPlayingWith(Connection *pPeer)
{
    if(pPeer == NULL && mpPlayingWith){
        QMapIterator<Connection*,ChatMsg*> iter(mChatMsgsWhilePlaying);
        while(iter.hasNext()){
            if(mPeers.contains(pPeer->peerAddress())){
                emit chatMsgCame(iter.key(),iter.value());
            }
            iter.next();
        }        
    }

    mChatMsgsWhilePlaying.clear();
    mpPlayingWith = pPeer;
    status(pPeer != NULL ? PeerViewInfoMsg::Busy : PeerViewInfoMsg::Free);
}
