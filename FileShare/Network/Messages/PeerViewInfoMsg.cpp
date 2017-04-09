#include "PeerViewInfoMsg.h"

int PeerViewInfoMsg::TypeID = 1;//qMetaTypeId<PeerViewInfoMsg>();

PeerViewInfoMsg::PeerViewInfoMsg()
{
}

PeerViewInfoMsg::PeerViewInfoMsg(QString name, QColor color):mName(name)
{
}

PeerViewInfoMsg::~PeerViewInfoMsg()
{
}

void PeerViewInfoMsg::setName(const QString name)
{
    mName = name;
}

QString PeerViewInfoMsg::name()const{return mName;}


void PeerViewInfoMsg::write(QDataStream &dataBuffer)
{
    dataBuffer << TypeID;
    dataBuffer << mName;
}

void PeerViewInfoMsg::read(QDataStream &dataBuffer)
{
    dataBuffer >> mName;
}

int PeerViewInfoMsg::typeId() { return PeerViewInfoMsg::TypeID;}

