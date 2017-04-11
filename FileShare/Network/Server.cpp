#include "Server.h"
#include "Connection.h"
#include <QDebug>

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
    Connection *pConnection = new Connection(sockId, this);
    qDebug() << " incomingConnection::newPeer fired";
    emit newPeer(pConnection);
}
