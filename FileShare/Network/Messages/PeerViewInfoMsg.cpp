#include "PeerViewInfoMsg.h"


PeerViewInfoMsg::PeerViewInfoMsg(const QString& name, int port,PeerStatus status, QObject* p)
    : Message(p)
    , _name(name)
    , _status(status)
    , _port(port)
{
}


void PeerViewInfoMsg::write(QDataStream &dataBuffer)
{
    dataBuffer << TypeID;
    dataBuffer << _name;
    dataBuffer << _port;
    dataBuffer << (int)_status;
}


void PeerViewInfoMsg::read(QDataStream &dataBuffer)
{
    dataBuffer >> _name;
    dataBuffer >> _port;
    int tmp;
    dataBuffer >> tmp;
    _status = (PeerStatus)tmp;
}


int PeerViewInfoMsg::typeId() { return PeerViewInfoMsg::TypeID;}

