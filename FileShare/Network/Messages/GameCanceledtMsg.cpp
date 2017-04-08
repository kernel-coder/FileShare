#include "GameCanceledMsg.h"

int GameCanceledtMsg::TypeID = 5;//qMetaTypeId<GameCanceledtMsg>();

GameCanceledtMsg::GameCanceledtMsg()
{
}


GameCanceledtMsg::GameCanceledtMsg(QString msg):mMsg(msg)
{
}

GameCanceledtMsg::~GameCanceledtMsg()
{

}

void GameCanceledtMsg::setString(const QString strData)
{
    mMsg = strData;
}

QString GameCanceledtMsg::string()const
{
    return mMsg;
}

void GameCanceledtMsg::write(QDataStream &dataBuffer)
{
    dataBuffer << TypeID;
    dataBuffer << mMsg;
}


void GameCanceledtMsg::read(QDataStream &dataBuffer)
{
    dataBuffer >> mMsg;
}

int GameCanceledtMsg::typeId(){return GameCanceledtMsg::TypeID;}
