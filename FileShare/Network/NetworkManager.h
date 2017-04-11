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

#define NetMgr NetworkManager::me()

class Connection;
class PeerManager;
class Message;
class ChatMsg;
class PlayRequestMsg;
class PlayRequestResultMsg;
class GameCanceledMsg;
class LineAddedMsg;

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

    bool sendMessage(Connection *conn, Message *msg);
    Connection *hasConnection(const QHostAddress &senderIp, int nSenderPort);

    void addPendingPeers(const QHostAddress &senderIp, int port, Connection* conn);
    void removePendingPeers(Connection* conn);
    Connection *hasPendingConnection(const QHostAddress &senderIp, int nSenderPort);

signals:
    void newParticipant(Connection *connection);
    void participantLeft(Connection *connection);
    void chatMsgCame(Connection *sender, ChatMsg *msg);
    void playRequestCame(Connection *sender, PlayRequestMsg *msg);
    void playRequestResultCame(Connection *sender, PlayRequestResultMsg *msg);
    void gameCanceledMsgCame(Connection *sender, GameCanceledMsg *msg);
    void lineAddedMsgCame(Connection *sender, LineAddedMsg *msg);
    void newMsgCame(Connection *sender, Message *msg);
    void newMsgCame();    

public slots:
    void broadcastUserInfoChanged();

private slots:
    void newConnection(Connection *connection);
    void connectionError(QAbstractSocket::SocketError socketError);
    void onDisconnected();
    void onReadyForUse();
    void onNewMessageArrived(Connection *conn, Message *msg);
    void closeAllSocks();

private:
    void removeConnection(Connection *conn);

    PeerManager *mpPeerManager;
    Server mServer;
    QHash<QString, Connection*> mPeers;
    QHash<QString, Connection*> mPendingPeers;
    typedef QMap<Connection*,ChatMsg*> ChatMsgMap;
    ChatMsgMap mChatMsgsWhilePlaying;
    Connection *mpPlayingWith;
};

#endif // NETWORKMANAGER_H
