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

class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    TcpSocket(QObject * p = nullptr);

signals:
    void newRawMsgArrived(const QByteArray& data);

private slots:
    void sendRawMessage(const QByteArray& data);

private slots:
    void onDataReadReady();

    void slotConnectToHost(const QHostAddress &address, quint16 port) {connectToHost(address, port);}
    void slotDisconnectFromHost() {disconnectFromHost();}
    void slotClose() {close();}

private:
     QMutex mMutex;
     qint64 mnBlockSize;
};

class Connection : public QThread
{
Q_OBJECT
public:
    Connection(int sockId = 0, QObject *parent = 0);
    ~Connection();
    MetaPropertyPrivateSet_Ex(PeerViewInfoMsg* , peerViewInfo)
    MetaPropertyPrivateSet_Ex(int , id)

public:
    TcpSocket* socket() {return mSocket;}
    void startAndWait();

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

    void fireSendRawMessage(const QByteArray& data);
    void readyForUse();
    void newMessageArrived(Connection *sender, Message *msg);

    void sigConnectToHost(const QHostAddress &address, quint16 port);
    void sigDisconnectFromHost();
    void sigClose();

public slots:
    void sendMessage(Message *msg);
    void sendClientViewInfo();

private slots:
    void onNewRawMessageReceived(const QByteArray& data);

protected:
    void run();

private:
    bool mPeerInfoSent;
    int mSockId;
    TcpSocket* mSocket;
    QMutex mMutex;
    QWaitCondition mWaitCondition;
};

#endif // CONNECTION_H
