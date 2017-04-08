#include "PeerViewInfoMsg.h"

int PeerViewInfoMsg::TypeID = 1;//qMetaTypeId<PeerViewInfoMsg>();

PeerViewInfoMsg::PeerViewInfoMsg()
{
}

PeerViewInfoMsg::PeerViewInfoMsg(QString name, QColor color):mName(name),mColor(color)
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

void PeerViewInfoMsg::setColor(QColor color)
{
    mColor = color;
}

QColor PeerViewInfoMsg::color()const{return mColor;}

void PeerViewInfoMsg::write(QDataStream &dataBuffer)
{
    dataBuffer << TypeID;
    dataBuffer << mName;
    dataBuffer << mColor.red() << mColor.green() << mColor.blue();
}

void PeerViewInfoMsg::read(QDataStream &dataBuffer)
{
    dataBuffer >> mName;
    int r,g,b;
    dataBuffer >> r >> g >> b;
    mColor = QColor::fromRgb(r,g,b);
}

int PeerViewInfoMsg::typeId() { return PeerViewInfoMsg::TypeID;}

