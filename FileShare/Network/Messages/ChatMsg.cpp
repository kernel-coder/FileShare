#include "ChatMsg.h"

int ChatMsg::TypeID = 2;//qMetaTypeId<ChatMsg>();

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

void ChatMsg::write(QDataStream &dataBuffer)
{
    dataBuffer << TypeID;
    dataBuffer << mMsg;
}


void ChatMsg::read(QDataStream &dataBuffer)
{
    dataBuffer >> mMsg;
}

int ChatMsg::typeId(){return ChatMsg::TypeID;}
