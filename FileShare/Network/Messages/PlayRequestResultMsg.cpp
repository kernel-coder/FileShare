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

void PlayRequestResultMsg::write(QDataStream &buf)
{
    buf << TypeID;
    buf << int(mStatus);
    buf << mStatusMsg;
    buf << mnDotsInRow;
    buf << mnDotsInColumn;
}

void PlayRequestResultMsg::read(QDataStream &buf)
{
    int nStatus;
    buf >> nStatus;
    mStatus = PlayRequestResultMsg::Status(nStatus);
    buf >> mStatusMsg;
    buf >> mnDotsInRow;
    buf >> mnDotsInColumn;
}

int PlayRequestResultMsg::typeId()
{
    return PlayRequestResultMsg::TypeID;
}

