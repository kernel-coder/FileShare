#ifndef CHATMSG_H
#define CHATMSG_H

#include "Message.h"

class ChatMsg : public Message
{
    Q_OBJECT
public:
    ChatMsg();
    ChatMsg(QString msg);
    ~ChatMsg();
    void setString(const QString strData);
    QString string()const;

    virtual void read(QDataStream &buf);
    virtual void write(QDataStream &buf);
    virtual int typeId();

public:
    static int TypeID;

private:
    QString mMsg;
};

#endif // CHATMSG_H
