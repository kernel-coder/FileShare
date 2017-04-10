#include "ServerInfoMsg.h"

ServerInfoMsg::ServerInfoMsg(int nPort, QObject* p)
    : Message(p)
    , _port(nPort)
{

}


void ServerInfoMsg::write(QDataStream &dataBuffer)
{
    dataBuffer << typeId();
    dataBuffer << _port;
}


void ServerInfoMsg::read(QDataStream &dataBuffer)
{
    dataBuffer >> _port;
}

int ServerInfoMsg::typeId() { return ServerInfoMsg::TypeID;}

