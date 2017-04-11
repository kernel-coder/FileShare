#include "ChatMsg.h"

int ChatMsg::TypeID = -1;//qMetaTypeId<ChatMsg>();

ChatMsg::ChatMsg()
{
}


ChatMsg::ChatMsg(QString msg):mMsg(msg)
{
}

ChatMsg::~ChatMsg()
{

}

void ChatMsg::setString(const QString strData)
{
    mMsg = strData;
}

QString ChatMsg::string()const
{
    return mMsg;
}

void ChatMsg::write(QDataStream &buf)
{
    buf << TypeID;
    buf << mMsg;
}


void ChatMsg::read(QDataStream &buf)
{
    buf >> mMsg;
}

int ChatMsg::typeId(){return ChatMsg::TypeID;}
