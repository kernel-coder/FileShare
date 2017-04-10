#include "ShareResponseMsg.h"

ShareResponseMsg::ShareResponseMsg(const QString& name, QObject* p)
    : Message(p)
    , _name(name)
{

}


void ShareResponseMsg::write(QDataStream &dataBuffer)
{
    dataBuffer << typeId();
    dataBuffer << _name;
}


void ShareResponseMsg::read(QDataStream &dataBuffer)
{
    dataBuffer >> _name;
}

int ShareResponseMsg::typeId() { return ShareResponseMsg::TypeID;}
