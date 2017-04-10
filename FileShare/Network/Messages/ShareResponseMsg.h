#pragma once
#include "Message.h"

class ShareResponseMsg : public Message
{
public:
    static int TypeID ;

    ShareResponseMsg(const QString& name = "", QObject* p = 0);
    MetaPropertyPublicSet_Ex(QString, name)

    virtual void read(QDataStream &dataBuffer);
    virtual void write(QDataStream &dataBuffer);
    virtual int typeId();
};
