#include "PlayRequestMsg.h"

int PlayRequestMsg::TypeID = 3;

PlayRequestMsg::PlayRequestMsg():mnDotsInRow(2),mnDotsInColumn(2)
{
}

PlayRequestMsg::PlayRequestMsg(int nDotsInRow, int nDotsInColumn):mnDotsInRow(nDotsInRow),mnDotsInColumn(nDotsInColumn)
{

}

void PlayRequestMsg::setDotsInRow(const int nDotsInRow)
{
    mnDotsInRow = nDotsInRow;
}

int PlayRequestMsg::dotsInRow()const{return mnDotsInRow;}

void PlayRequestMsg::setDotsInColumn(const int nDotsInColumn)
{
    mnDotsInColumn = nDotsInColumn;
}

int PlayRequestMsg::dotsInColumn()const{return mnDotsInColumn;}

void PlayRequestMsg::setString(const QString strData)
{
    mRequestMsg = strData;
}

QString PlayRequestMsg::string()const
{
    return mRequestMsg;
}

void PlayRequestMsg::write(QDataStream &buf)
{
    buf << TypeID;
    buf << mnDotsInRow;
    buf << mnDotsInColumn;
    buf << mRequestMsg;
}

void PlayRequestMsg::read(QDataStream &buf)
{
    buf >> mnDotsInRow;
    buf >> mnDotsInColumn;
    buf >> mRequestMsg;
}

int PlayRequestMsg::typeId() { return PlayRequestMsg::TypeID;}
