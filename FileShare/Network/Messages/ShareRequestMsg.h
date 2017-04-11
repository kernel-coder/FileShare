#pragma once

#include "Message.h"
#include <QStringList>

class ShareRequestMsg : public Message
{
public:
    static int TypeID ;

    ShareRequestMsg(const QStringList& files = QStringList(), QObject* p = 0);
    MetaPropertyPublicSet_Ex(QStringList, files)

    virtual void read(QDataStream &dataBuffer);
    virtual void write(QDataStream &dataBuffer);
    virtual int typeId();
};
