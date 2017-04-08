#ifndef PLAYREQUESTMSG_H
#define PLAYREQUESTMSG_H

#include "Message.h"

class PlayRequestMsg : public Message
{
    Q_OBJECT
public:
    PlayRequestMsg();
    PlayRequestMsg(int nDotsInRow,int nDotsInColumn);

    void setDotsInRow(const int nDotsInRow);
    int dotsInRow()const;
    void setDotsInColumn(const int nDotsInColumn);
    int dotsInColumn()const;
    void setString(const QString strData);
    QString string()const;

    virtual void read(QDataStream &dataBuffer);
    virtual void write(QDataStream &dataBuffer);
    virtual int typeId();

public:
    static int TypeID;

private:
    int mnDotsInRow,mnDotsInColumn;
    QString mRequestMsg;
};

#endif // PLAYREQUESTMSG_H
