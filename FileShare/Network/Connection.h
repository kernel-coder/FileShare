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
    Connection(QObject *parent = 0);
    static Connection* createConnection(QObject* parent);
    void setupSocket(int sockId = 0);
    MetaPropertyPrivateSet_Ex(PeerViewInfoMsg* , peerViewInfo)

signals:
    void readyForUse();
    void peerViewInfoChanged();
    void newMessageArrived(Connection *sender, Message *msg);
    void fireSendMessage(Message* msg);

public slots:
    bool sendMessage(Message *pMsg);
    void sendClientViewInfo();

private slots:
    void dataReadyToRead();
    void sendMessage_Private(Message *pMsg);

private:
    quint64 mBlockSize;
    QMutex mMutex;
};

#endif // CONNECTION_H
