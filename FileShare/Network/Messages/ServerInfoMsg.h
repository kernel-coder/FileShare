#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <QObject>
#include "Message.h"

class ServerInfoMsg : public Message
{
    Q_OBJECT
public:
    ServerInfoMsg(int nPort = 0, QObject* p =0);
        
    MetaPropertyPublicSet_Ex(int, port)

    virtual void read(QDataStream &dataBuffer);
    virtual void write(QDataStream &dataBuffer);
    virtual int typeId();

    static int TypeID;

private:
};

#endif // SERVERINFO_H
