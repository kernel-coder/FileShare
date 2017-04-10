#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QMutex>
#include "Messages/PeerViewInfoMsg.h"
#include "JObject.h"

class Message;
class NetworkManager;

class Connection : public QTcpSocket
{
Q_OBJECT
public:
    Connection(int sockId = 0, QObject *parent = 0);
    bool sendMessage(Message *pMsg);
    MetaPropertyPrivateSet_Ex(PeerViewInfoMsg* , peerViewInfo)

signals:
    void readyForUse();
    void peerViewInfoChanged();
    void newMessageArrived(Connection *pFrom,Message *pMsg);

public slots:
    void sendClientViewInfo();

private slots:
    void dataReadyToRead();

private:
    void setPeerViewInfo(PeerViewInfoMsg *pPeerViewInfo);

private:
    quint64 mBlockSize;
    QMutex mMutex;
};

#endif // CONNECTION_H
