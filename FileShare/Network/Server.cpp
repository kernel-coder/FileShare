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
    addPendingConnection(conn->socket());
    NetMgr->addPendingPeers(conn->peerAddress(), conn->peerPort(), conn);
    qDebug() << " incomingConnection::newPeer firing..." << conn->peerAddress().toString() << conn->peerPort();
    emit newPeer(conn);
//    if (NetMgr->hasPendingConnection(conn->peerAddress(), conn->peerPort()) == NULL
//            && NetMgr->hasConnection(conn->peerAddress(), conn->peerPort()) == NULL) {
//        NetMgr->addPendingPeers(conn->peerAddress(), conn->peerPort(), conn);
//        qDebug() << " incomingConnection::newPeer firing...";
//        emit newPeer(conn);
//    }
//    else {
//        conn->close();
//        conn->deleteLater();
//        qDebug() << " incomingConnection::newPeer existing connecting... closing";
//    }
}
