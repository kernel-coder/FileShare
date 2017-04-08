#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <QObject>
#include "Message.h"

class ServerInfoMsg : public Message
{
    Q_OBJECT
public:
    enum MyStatus {Busy,Free};
    
    ServerInfoMsg();
    ServerInfoMsg(const int nPort, MyStatus status);
        
    void setPort(const int nPort);
    int port()const;
    void setStatus(MyStatus status);
    MyStatus status();

    virtual void read(QDataStream &dataBuffer);
    virtual void write(QDataStream &dataBuffer);
    virtual int typeId();

    static int TypeID;

private:
    int mnPort;
    MyStatus mStatus;

};

#endif // SERVERINFO_H
