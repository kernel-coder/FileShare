#include "ShareRequestMsg.h"

ShareRequestMsg::ShareRequestMsg(const QString& name, QObject* p)
    : Message(p)
    , _name(name)
{

}


void ShareRequestMsg::write(QDataStream &dataBuffer)
{
    dataBuffer << typeId();
    dataBuffer << _name;
}


void ShareRequestMsg::read(QDataStream &dataBuffer)
{
    dataBuffer >> _name;
}

int ShareRequestMsg::typeId() { return ShareRequestMsg::TypeID;}

