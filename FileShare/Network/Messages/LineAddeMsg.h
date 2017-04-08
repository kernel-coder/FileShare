#ifndef LINEADDEMSG_H
#define LINEADDEMSG_H

#include "Message.h"

class LineAddeMsg : public Message
{
    Q_OBJECT
public:
    LineAddeMsg();
    LineAddeMsg();
    virtual void read(QDataStream &dataBuffer);
    virtual void write(QDataStream &dataBuffer);
    virtual int typeId();

public:
    static int TypeID;

private:
    int mnDotsInRow,mnDotsInColumn;
};

#endif // LINEADDEMSG_H
