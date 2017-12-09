#pragma once

#include "Message.h"

class TransferStatusFlag : public QObject {
    Q_OBJECT
public:
    TransferStatusFlag(QObject* p = 0) : QObject(p){}
    enum ControlStatus {NotStarted, Running, Pause, Stop, Delete, Failed, Finished};
    Q_ENUMS(ControlStatus)
};

class TransferControlMsg : public Message
{
    Q_OBJECT
public:
    static int TypeID ;
    TransferControlMsg(QObject* p = 0);

    MetaPropertyPublicSet_Ex(TransferStatusFlag::ControlStatus, status)
    MetaPropertyPublicSet_Ex(QString, transferId)

    virtual void read(QDataStream &buf);
    virtual void write(QDataStream &buf);
    virtual int typeId();
};
