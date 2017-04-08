#ifndef PLAYREQUESTRESULTMSG_H
#define PLAYREQUESTRESULTMSG_H

#include "Message.h"

class PlayRequestResultMsg : public Message
{    
    Q_OBJECT
public:
    enum Status { Rejected,Accepted};
    
    PlayRequestResultMsg();
    PlayRequestResultMsg(PlayRequestResultMsg::Status status);
    ~PlayRequestResultMsg();

    void setStatus(PlayRequestResultMsg::Status status);
    PlayRequestResultMsg::Status status();
    void setString(const QString strData);
    QString string()const;
    void setDotsInRow(const int nDotsInRow);
    int dotsInRow()const;
    void setDotsInColumn(const int nDotsInColumn);
    int dotsInColumn()const;

    virtual void read(QDataStream &dataBuffer);
    virtual void write(QDataStream &dataBuffer);
    virtual int typeId();

public:
    static int TypeID;

private:
    Status mStatus;
    QString mStatusMsg;
    int mnDotsInRow,mnDotsInColumn;
};


#endif // PLAYREQUESTRESULTMSG_H
