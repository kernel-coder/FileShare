#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QMutex>
#include "Messages/PeerViewInfoMsg.h"
#include "JObject.h"
#include <QHostAddress>
#include <QThread>
#include <QWaitCondition>

class Message;
class NetworkManager;


class Connection : public QTcpSocket
{
Q_OBJECT
public:
    Connection(QObject *parent = 0);
    ~Connection();
    MetaPropertyPrivateSet_Ex(PeerViewInfoMsg* , peerViewInfo)
    MetaPropertyPrivateSet_Ex(int , id)

signals:
    void readyForUse();
    void newMessageArrived(Connection *sender, Message *msg);

public slots:
    void sendMessage(Message *msg);
    void sendClientViewInfo();

private slots:
    void onConnected();
    void onDataReadReady();

private:
    bool mPeerInfoSent;
    QMutex mMutex;
    qint64 mMsgSize;
};

#endif // CONNECTION_H
