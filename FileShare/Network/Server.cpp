#include "Server.h"
#include "Connection.h"
#include <QDebug>
#include "NetworkManager.h"

Server::Server(QObject *parent) :
    QTcpServer(parent)
{
    listen(QHostAddress::Any);
}


Server::~Server()
{
    close();
}


void Server::incomingConnection(int sockId)
{
    Connection *conn = new Connection(sockId, this);
    connect(conn, SIGNAL(connected()), this, SLOT(onPeerConnectedInServer()));
    conn->startAndWait();
    qDebug() << "Adding pending peer from server " << conn->peerAddress().toString() << conn->peerPort();
    NetMgr->addPendingPeers(conn->peerAddress(), conn->peerPort(), conn);
    addPendingConnection(conn->socket());       
}


void Server::onPeerConnectedInServer()
{
    Connection *conn = qobject_cast<Connection*>(sender());
    if(conn) {
        qDebug() << "Peer conneciton success from server " <<  conn->peerAddress().toString() << conn->peerPort();
        conn->disconnect(this);
        emit newPeer(conn);
    }
}
