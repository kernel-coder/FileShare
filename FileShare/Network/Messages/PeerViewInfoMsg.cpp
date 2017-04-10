#include "PeerViewInfoMsg.h"


PeerViewInfoMsg::PeerViewInfoMsg(const QString& name, PeerStatus status, QObject* p)
    : Message(p)
    , _name(name)
    , _status(status)
{
}

void PeerViewInfoMsg::write(QDataStream &dataBuffer)
{
    dataBuffer << TypeID;
    dataBuffer << _name;
    dataBuffer << (int)_status;
}

void PeerViewInfoMsg::read(QDataStream &dataBuffer)
{
    dataBuffer >> _name;
    int tmp;
    dataBuffer >> tmp;
    _status = (PeerStatus)tmp;
}

int PeerViewInfoMsg::typeId() { return PeerViewInfoMsg::TypeID;}

