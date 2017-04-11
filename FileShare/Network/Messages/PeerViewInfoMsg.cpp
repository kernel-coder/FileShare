#include "PeerViewInfoMsg.h"


PeerViewInfoMsg::PeerViewInfoMsg(const QString& name, int port,PeerStatus status, QObject* p)
    : Message(p)
    , _name(name)
    , _status(status)
    , _port(port)
{
}


void PeerViewInfoMsg::write(QDataStream &buf)
{
    buf << TypeID;
    buf << _name;
    buf << _port;
    buf << (int)_status;
}


void PeerViewInfoMsg::read(QDataStream &buf)
{
    buf >> _name;
    buf >> _port;
    int tmp;
    buf >> tmp;
    _status = (PeerStatus)tmp;
}


int PeerViewInfoMsg::typeId() { return PeerViewInfoMsg::TypeID;}

