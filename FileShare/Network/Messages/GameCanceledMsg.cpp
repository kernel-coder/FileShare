#include "GameCanceledMsg.h"


int GameCanceledMsg::TypeID = 5;//qMetaTypeId<GameCanceledMsg>();

GameCanceledMsg::GameCanceledMsg()
{
}


GameCanceledMsg::GameCanceledMsg(QString msg):mMsg(msg)
{
}

GameCanceledMsg::~GameCanceledMsg()
{

}

void GameCanceledMsg::setString(const QString strData)
{
    mMsg = strData;
}

QString GameCanceledMsg::string()const
{
    return mMsg;
}

void GameCanceledMsg::write(QDataStream &buf)
{
    buf << TypeID;
    buf << mMsg;
}


void GameCanceledMsg::read(QDataStream &buf)
{
    buf >> mMsg;
}

int GameCanceledMsg::typeId(){return GameCanceledMsg::TypeID;}
