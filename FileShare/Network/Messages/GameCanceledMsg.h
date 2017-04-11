#pragma once

#include "Message.h"

class GameCanceledMsg : public Message
{
    Q_OBJECT
public:
    GameCanceledMsg();
    GameCanceledMsg(QString strMsg);
    ~GameCanceledMsg();

    void setString(const QString strData);
    QString string()const;

    virtual void read(QDataStream &buf);
    virtual void write(QDataStream &buf);
    virtual int typeId();

public:
    static  int TypeID;

private:
    QString mMsg;
};

