#include "PeerViewInfoMsg.h"


PeerViewInfoMsg::PeerViewInfoMsg(const QString& name, int port,PeerStatus status
                                 , const QString& deviceId, QObject* p)
    : Message(p)
    , _name(name)
    , _status(status)
    , _port(port)
    , _deviceId(deviceId)
{
}


void PeerViewInfoMsg::write(QDataStream &buf)
{
    buf << TypeID;
    buf << _name;
    buf << _port;
    buf << (int)_status;
    buf << _deviceId;
}


void PeerViewInfoMsg::read(QDataStream &buf)
{
    buf >> _name;
    buf >> _port;
    int tmp;
    buf >> tmp;
    _status = (PeerStatus)tmp;
    buf >> _deviceId;
}


int PeerViewInfoMsg::typeId() { return PeerViewInfoMsg::TypeID;}

