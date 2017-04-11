#include "ServerInfoMsg.h"

ServerInfoMsg::ServerInfoMsg(int nPort, QObject* p)
    : Message(p)
    , _port(nPort)
{

}


void ServerInfoMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << _port;
}


void ServerInfoMsg::read(QDataStream &buf)
{
    buf >> _port;
}

int ServerInfoMsg::typeId() { return ServerInfoMsg::TypeID;}

