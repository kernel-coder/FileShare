#include "Server.h"
#include "Connection.h"
#include <QDebug>
#include "NetworkManager.h"

Server::Server(QObject *parent) :
    QTcpServer(parent)
{
    if (listen(QHostAddress::Any)) {
        qDebug() << QString("server listening: %1:%2").arg(serverAddress().toString()).arg(serverPort());
    }
    else {
        qDebug() << "server listening failed: " << errorString();
    }
}


Server::~Server()
{
    close();
}


void Server::onAcceptError(QAbstractSocket::SocketError socketError)
{
    qWarning() << "server accepting error: " << socketError << ", " << errorString();
}


void Server::incomingConnection(int sockId)
{
    Connection *conn = NetMgr->createConnection();    
    if (!conn->setSocketDescriptor(sockId) || NetMgr->hasConnection(conn->peerAddress(), conn->peerPort())) {
        qDebug() << "Destroying socket from server " << conn->peerAddress().toString() << conn->peerPort();
        conn->close();
        conn->deleteLater();
    }
    else {
        NetMgr->replacePendingPeer(conn->peerAddress(), conn->peerPort(), conn);
        connect(conn, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onPeerConnectingError(QAbstractSocket::SocketError)));
        qDebug() << "Adding pending peer from server " << conn->peerAddress().toString() << conn->peerPort();
        emit newPeer(conn);
    }
}


void Server::onPeerConnectedInServer()
{
    Connection *conn = qobject_cast<Connection*>(sender());
    if(conn) {
        qDebug() << "Peer conneciton success from server " <<  conn->peerAddress().toString() << conn->peerPort();
        disconnect(conn, SIGNAL(connected()), this, SLOT(onPeerConnectedInServer()));
        disconnect(conn, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onPeerConnectingError(QAbstractSocket::SocketError)));
        emit newPeer(conn);
    }
}


void Server::onPeerConnectingError(QAbstractSocket::SocketError socketError)
{
    Connection *conn = qobject_cast<Connection*>(sender());
    if(conn) {
        qDebug() << "Peer conneciton error " <<  conn->errorString();
        disconnect(conn, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onPeerConnectingError(QAbstractSocket::SocketError)));
        NetMgr->removePendingPeers(conn);
        conn->deleteLater();
    }
}

