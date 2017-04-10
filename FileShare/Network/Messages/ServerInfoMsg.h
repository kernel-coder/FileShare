#pragma once

#include "Message.h"

class ServerInfoMsg : public Message
{
    Q_OBJECT
public:
    static int TypeID;
    ServerInfoMsg(int nPort = 0, QObject* p =0);
        
    MetaPropertyPublicSet_Ex(int, port)

    virtual void read(QDataStream &dataBuffer);
    virtual void write(QDataStream &dataBuffer);
    virtual int typeId();



private:
};

