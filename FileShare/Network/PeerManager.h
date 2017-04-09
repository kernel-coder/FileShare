#ifndef PEERMANAGER_H
#define PEERMANAGER_H

#include <QObject>
#include <QHostAddress>
#include <QByteArray>
#include <QUdpSocket>
#include <QTimer>
#include <QMutex>
#include <QHash>

class NetworkManager;
class Connection;
class ServerInfoMsg;

class PeerManager : public QObject
{
Q_OBJECT
public:
    PeerManager(NetworkManager *pNetManager, QObject *parent = 0);
    void setServerPort(int nPort);
    QByteArray userName() const;
    void startBroadcasting();
    bool isLocalHostAddress(const QHostAddress &address);

signals:
    void newPeer(Connection *pConnection);

public slots:
    void connected();

private slots:
    void sendBroadcastDatagram();
    void readBroadcastDatagram();

private:
    void updateAddresses();
    QByteArray serverInfo();

private:
    NetworkManager *mpNetManager;
    QList<QHostAddress> mBroadcastAddresses;
    QList<QHostAddress> mIPAddresses;
    QUdpSocket mBroadcastSocket;
    QTimer mBroadcastTimer;
    int mnServerPort;
    QMutex mMutex;
};

#endif // PEERMANAGER_H
