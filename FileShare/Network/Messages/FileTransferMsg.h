#pragma once

#include "Message.h"

class FileTransferMsg : public Message
{
public:
    static int TypeID ;
    FileTransferMsg(const QString& filename = "", QObject* p =0);

    MetaPropertyPublicSet_Ex(QString, filename)
    virtual void read(QDataStream &buf);
    virtual void write(QDataStream &buf);
    virtual int typeId();
};
