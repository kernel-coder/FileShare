#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

class Connection;

class Server : public QTcpServer
{
Q_OBJECT
public:
    Server(QObject *parent = 0);
    ~Server();

signals:
    void newPeer(Connection *pConnection);

protected:
    void incomingConnection(int sockId);

private slots:
    void onAcceptError(QAbstractSocket::SocketError socketError);
    void onPeerConnectedInServer();
    void onPeerConnectingError(QAbstractSocket::SocketError socketError);

};

#endif // SERVER_H
