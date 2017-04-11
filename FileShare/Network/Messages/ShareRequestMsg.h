#pragma once

#include "Message.h"
#include <QStringList>

class ShareRequestMsg : public Message
{
public:
    static int TypeID ;

    ShareRequestMsg(int requestId = -1, const QStringList& files = QStringList(), QObject* p = 0);

    MetaPropertyPublicSet_Ex(int, requestId)
    MetaPropertyPublicSet_Ex(QStringList, files)

    virtual void read(QDataStream &buf);
    virtual void write(QDataStream &buf);
    virtual int typeId();
};
