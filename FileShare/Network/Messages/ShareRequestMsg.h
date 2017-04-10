#pragma once
#include "Message.h"

class ShareRequestMsg : public Message
{
public:
    static int TypeID ;

    ShareRequestMsg(const QString& name = "", QObject* p = 0);
    MetaPropertyPublicSet_Ex(QString, name)

    virtual void read(QDataStream &dataBuffer);
    virtual void write(QDataStream &dataBuffer);
    virtual int typeId();
};
