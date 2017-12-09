#include "TransferControlMsg.h"

TransferControlMsg::TransferControlMsg(QObject* p)
    : Message(p)
{
}


void TransferControlMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << (int)_status;
    buf << _transferId;
}


void TransferControlMsg::read(QDataStream &buf)
{
    int status;
    buf >> status;
    _status = (TransferStatusFlag::ControlStatus)status;
    buf >> _transferId;
}


int TransferControlMsg::typeId() { return TransferControlMsg::TypeID;}
