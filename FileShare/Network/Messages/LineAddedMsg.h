#ifndef LINEADDEDMSG_H
#define LINEADDEDMSG_H

#include "Message.h"

class LineAddedMsg : public Message
{
    Q_OBJECT
public:
    LineAddedMsg();
    LineAddedMsg(uint nDot1Row,uint nDot1Column,uint nDot2Row,uint nDot2Column);
    ~LineAddedMsg();

    void setDot1Row(const uint nRow);
    uint dot1Row()const;
    void setDot1Column(const uint nColumn);
    uint dot1Column()const;
    void setDot2Row(const uint nRow);
    uint dot2Row()const;
    void setDot2Column(const uint nColumn);
    uint dot2Column()const;

    virtual void read(QDataStream &dataBuffer);
    virtual void write(QDataStream &dataBuffer);
    virtual int typeId();

public:
    static int TypeID;

private:
    uint mnDot1Row,mnDot1Column;
    uint mnDot2Row,mnDot2Column;
};

#endif // LINEADDEDMSG_H
