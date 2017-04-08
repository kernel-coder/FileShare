#include "PlayRequestResultMsg.h"

int PlayRequestResultMsg::TypeID = 4;

PlayRequestResultMsg::PlayRequestResultMsg():mStatus(PlayRequestResultMsg::Rejected)
{
}

PlayRequestResultMsg::PlayRequestResultMsg(PlayRequestResultMsg::Status status):mStatus(status)
{

}

PlayRequestResultMsg::~PlayRequestResultMsg()
{

}

void PlayRequestResultMsg::setStatus(PlayRequestResultMsg::Status status)
{
    mStatus = status;
}

PlayRequestResultMsg::Status PlayRequestResultMsg::status()
{
    return mStatus;
}

void PlayRequestResultMsg::setString(const QString strData)
{
    mStatusMsg = strData;
}

QString PlayRequestResultMsg::string()const
{
    return mStatusMsg;
}

void PlayRequestResultMsg::setDotsInRow(const int nDotsInRow)
{
    mnDotsInRow = nDotsInRow;
}

int PlayRequestResultMsg::dotsInRow()const{return mnDotsInRow;}

void PlayRequestResultMsg::setDotsInColumn(const int nDotsInColumn)
{
    mnDotsInColumn = nDotsInColumn;
}

int PlayRequestResultMsg::dotsInColumn()const{return mnDotsInColumn;}

void PlayRequestResultMsg::write(QDataStream &dataBuffer)
{
    dataBuffer << TypeID;
    dataBuffer << int(mStatus);
    dataBuffer << mStatusMsg;
    dataBuffer << mnDotsInRow;
    dataBuffer << mnDotsInColumn;
}

void PlayRequestResultMsg::read(QDataStream &dataBuffer)
{
    int nStatus;
    dataBuffer >> nStatus;
    mStatus = PlayRequestResultMsg::Status(nStatus);
    dataBuffer >> mStatusMsg;
    dataBuffer >> mnDotsInRow;
    dataBuffer >> mnDotsInColumn;
}

int PlayRequestResultMsg::typeId()
{
    return PlayRequestResultMsg::TypeID;
}

