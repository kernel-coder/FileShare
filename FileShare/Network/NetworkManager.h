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

    Connection* createConnection(int sockId = 0);
    void connectionHandShakeFialed();

    MetaPropertyPublicSet_Ex(QString, username)
    MetaPropertyPublicSet_Ex(QString, ip)
    MetaPropertyPublicSet_Ex(int, port)
    MetaPropertyPublicSet_Ex(PeerViewInfoMsg::PeerStatus, status)

    void sendMessage(Connection *conn, Message *msg);
    Connection *hasConnection(const QHostAddress &senderIp, int nSenderPort);

    void addPendingPeers(const QHostAddress &senderIp, int port, Connection* conn);
    void removePendingPeers(Connection* conn);
    Connection *hasPendingConnection(const QHostAddress &senderIp, int nSenderPort);

signals:
    void newParticipant(Connection *connection);
    void participantLeft(Connection *connection);
    void newMsgCome(Connection *sender, Message *msg);

public slots:
    void connectManual(const QString& host, int port);
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
