#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QMutex>
#include "Messages/ServerInfoMsg.h"

class Message;
class PeerViewInfoMsg;
class NetworkManager;

class Connection : public QTcpSocket
{
Q_OBJECT
public:
    Connection(int nSocketDesciptor,ServerInfoMsg::MyStatus status = ServerInfoMsg::Free, QObject *parent = 0);
    Connection(ServerInfoMsg::MyStatus status = ServerInfoMsg::Free, QObject *parent = 0);
    bool sendMessage(Message *pMsg);
    PeerViewInfoMsg *peerViewInfo()const;
    ServerInfoMsg::MyStatus status();
    void setStatus(ServerInfoMsg::MyStatus status);

signals:
    void readyForUse();
    void peerViewInfoChanged();
    void newMessageArrived(Connection *pFrom,Message *pMsg);
    void statusChanged(Connection *pMe);

public slots:
    void sendClientViewInfo();

private slots:
    void dataReadyToRead();

private:
    void setPeerViewInfo(PeerViewInfoMsg *pPeerViewInfo);

private:
    quint16 mnBlockSize;
    PeerViewInfoMsg *mpPeerViewInfo;
    ServerInfoMsg::MyStatus mStatus;
    QMutex mMutex;
};

#endif // CONNECTION_H
