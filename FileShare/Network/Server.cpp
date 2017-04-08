#include "Server.h"
#include "Connection.h"

Server::Server(QObject *parent) :
    QTcpServer(parent)
{
    listen(QHostAddress::Any);
}

Server::~Server()
{
    close();
}

void Server::incomingConnection(int nSocketDescriptor)
{
    Connection *pConnection = new Connection(nSocketDescriptor);
    emit newPeer(pConnection);
}
