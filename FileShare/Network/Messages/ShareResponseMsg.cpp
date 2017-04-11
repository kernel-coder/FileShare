#include "ShareResponseMsg.h"

ShareResponseMsg::ShareResponseMsg(int requestId, bool accepted, QObject* p)
    : Message(p)
    , _requestId(requestId)
    , _accepted(accepted)
{

}


void ShareResponseMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << _requestId;
    buf << _accepted;
}


void ShareResponseMsg::read(QDataStream &buf)
{
    buf >> _requestId;
    buf >> _accepted;
}


int ShareResponseMsg::typeId() { return ShareResponseMsg::TypeID;}
