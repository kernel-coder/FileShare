#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QMutex>
#include "Messages/PeerViewInfoMsg.h"
#include "JObject.h"
#include <QHostAddress>

class Message;
class NetworkManager;

class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    TcpSocket(QObject * p = nullptr);
    void setupSocket(int sockId = 0);

signals:
    void newMessageCome(Message *msg);

public slots:
    void sendMessage(Message *msg);
    void sendClientViewInfo();

private slots:
    void onDataReadReady();

    void slotConnectToHost(const QHostAddress &address, quint16 port) {connectToHost(address, port);}
    void slotDisconnectFromHost() {disconnectFromHost();}
    void slotClose() {close();}

private:
     QMutex mMutex;
     quint64 mnBlockSize;
};

class Connection : public QObject
{
Q_OBJECT
public:
    Connection(int sockId = 0, QObject *parent = 0);
    ~Connection();
    MetaPropertyPrivateSet_Ex(PeerViewInfoMsg* , peerViewInfo)

public:
    TcpSocket* socket() {return mSocket;}

    QHostAddress peerAddress() const {return mSocket->peerAddress();}
    quint16 peerPort() const {return mSocket->peerPort();}
    QString peerName() const {return mSocket->peerName();}
    QAbstractSocket::SocketError error() const {return mSocket->error();}
    QString errorString() const {return mSocket->errorString();}

    void connectToHost(const QHostAddress &address, quint16 port){emit sigConnectToHost(address, port);}
    void disconnectFromHost() {emit sigDisconnectFromHost();}
    void close() {emit sigClose();}

signals:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);

    void fireSendMessage(Message* msg);
    void readyForUse();
    void newMessageArrived(Connection *sender, Message *msg);

    void sigConnectToHost(const QHostAddress &address, quint16 port);
    void sigDisconnectFromHost();
    void sigClose();

public slots:
    void sendMessage(Message *msg);
    void sendClientViewInfo();

private slots:
    void onMessageReceived(Message* msg);

private:
    TcpSocket* mSocket;
};

#endif // CONNECTION_H
