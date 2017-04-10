#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "JObject.h"
#include <QAbstractSocket>
#include <QHash>
#include <QHostAddress>
#include <QMap>
#include <QHostInfo>
#include "Server.h"
#include "Messages/PeerViewInfoMsg.h"

class Connection;
class PeerManager;
class Message;
class ChatMsg;
class PlayRequestMsg;
class PlayRequestResultMsg;
class GameCanceledMsg;
class LineAddedMsg;
class DotController;


class NetworkManager : public QObject
{
Q_OBJECT
    explicit NetworkManager(QObject *parent = 0);
public:    
    static NetworkManager* me();
    ~NetworkManager();

    MetaPropertyPublicSet_Ex(QString, username)
    MetaPropertyPublicSet_Ex(QString, ip)
    MetaPropertyPublicSet_Ex(int, port)
    MetaPropertyPublicSet_Ex(PeerViewInfoMsg::PeerStatus, status)

    bool sendMessage(Connection *pConn, Message *pMsg);
    Connection *hasConnection(const QHostAddress &senderIp, int nSenderPort = -1);

    void addPendingPeers(const QHostAddress &senderIp, Connection* conn);
    void removePendingPeers(Connection* conn);
    Connection *hasPendingConnection(const QHostAddress &senderIp, int nSenderPort = -1);

    void setPlayingWith(Connection *pPeer);

signals:
    void newParticipant(Connection *connection);
    void participantLeft(Connection *pConn);
    void chatMsgCame(Connection *pFrom,ChatMsg *pMsg);
    void playRequestCame(Connection *pFrom,PlayRequestMsg *pMsg);
    void playRequestResultCame(Connection *pFrom,PlayRequestResultMsg *pMsg);
    void gameCanceledMsgCame(Connection *pFrom,GameCanceledMsg *pMsg);
    void lineAddedMsgCame(Connection *pFrom, LineAddedMsg *pMsg);
    void newMsgCame(Connection *pFrom, Message *pMsg);
    void newMsgCame();

private slots:
    void onLocalHostLookupDone(const QHostInfo &host);
    void newConnection(Connection *connection);
    void connectionError(QAbstractSocket::SocketError socketError);
    void disconnected();
    void readyForUse();
    void newMessageArrived(Connection *pConn, Message *pMsg);
    void closeAllSocks();
    void checkPCPlayerInfoChanged();

private:
    void removeConnection(Connection *pConnection);
    void disconnectSignal(Connection *pConnection);

    PeerManager *mpPeerManager;
    Server mServer;
    QMultiHash<QHostAddress, Connection *> mPeers;
    QMultiHash<QHostAddress, Connection *> mPendingPeers;
    typedef QMap<Connection*,ChatMsg*> ChatMsgMap;
    ChatMsgMap mChatMsgsWhilePlaying;
    Connection *mpPlayingWith;
};

#endif // NETWORKMANAGER_H
