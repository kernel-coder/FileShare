#pragma once
#include "Message.h"

class ShareResponseMsg : public Message
{
public:
    static int TypeID ;

    ShareResponseMsg(int requestId = -1, bool accepted = false, QObject* p = 0);

    MetaPropertyPublicSet_Ex(int, requestId)
    MetaPropertyPublicSet_Ex(bool, accepted)

    virtual void read(QDataStream &buf);
    virtual void write(QDataStream &buf);
    virtual int typeId();
};
