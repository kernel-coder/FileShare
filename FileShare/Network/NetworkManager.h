#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QAbstractSocket>
#include <QHash>
#include <QHostAddress>
#include <QMap>
#include "Server.h"
#include "Messages/ServerInfoMsg.h"

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
public:
    explicit NetworkManager(QObject *parent = 0);
    ~NetworkManager();

    bool sendMessage(Connection *pConn, Message *pMsg);
    Connection *hasConnection(const QHostAddress &senderIp, int nSenderPort = -1);
    ServerInfoMsg::MyStatus status();
    void setPlayingWith(Connection *pPeer);

signals:
    void newParticipant(Connection *pConn);
    void participantLeft(Connection *pConn);
    void chatMsgCame(Connection *pFrom,ChatMsg *pMsg);
    void playRequestCame(Connection *pFrom,PlayRequestMsg *pMsg);
    void playRequestResultCame(Connection *pFrom,PlayRequestResultMsg *pMsg);
    void gameCanceledMsgCame(Connection *pFrom,GameCanceledMsg *pMsg);
    void lineAddedMsgCame(Connection *pFrom, LineAddedMsg *pMsg);
    void newMsgCame(Connection *pFrom, Message *pMsg);
    void newMsgCame();

private slots:
    void newConnection(Connection *pConnection);
    void connectionError(QAbstractSocket::SocketError socketError);
    void disconnected();
    void readyForUse();
    void newMessageArrived(Connection *pConn,Message *pMsg);
    void closeAllSocks();
    void checkPCPlayerInfoChanged();

private:
    void removeConnection(Connection *pConnection);
    void disconnectSignal(Connection *pConnection);
    void setStatus(ServerInfoMsg::MyStatus status);

    PeerManager *mpPeerManager;
    Server mServer;
    QMultiHash<QHostAddress, Connection *> mPeers;
    typedef QMap<Connection*,ChatMsg*> ChatMsgMap;
    ChatMsgMap mChatMsgsWhilePlaying;
    ServerInfoMsg::MyStatus mStatus;
    Connection *mpPlayingWith;

    friend class DotController;

};

#endif // NETWORKMANAGER_H
