#include "ServerInfoMsg.h"

int ServerInfoMsg::TypeID = 0;

ServerInfoMsg::ServerInfoMsg()
{
}

ServerInfoMsg::ServerInfoMsg(const int nPort, MyStatus status):mnPort(nPort),mStatus(status)
{

}

void ServerInfoMsg::setPort(const int nPort)
{
    mnPort = nPort;
}

int ServerInfoMsg::port()const { return mnPort;}

void ServerInfoMsg::setStatus(ServerInfoMsg::MyStatus status)
{
    mStatus = status;
}

ServerInfoMsg::MyStatus ServerInfoMsg::status()
{
    return mStatus;
}

void ServerInfoMsg::write(QDataStream &dataBuffer)
{
    dataBuffer << typeId();
    dataBuffer << mnPort;
    dataBuffer << int(mStatus);
}

void ServerInfoMsg::read(QDataStream &dataBuffer)
{
    dataBuffer >> mnPort;
    int nStatus;
    dataBuffer >> nStatus;
    mStatus = ServerInfoMsg::MyStatus(nStatus);
}

int ServerInfoMsg::typeId() { return ServerInfoMsg::TypeID;}

