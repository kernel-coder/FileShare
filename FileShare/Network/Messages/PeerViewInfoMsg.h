#pragma once

#include "Message.h"
#include <QColor>

class PeerViewInfoMsg : public Message
{
    Q_OBJECT
public:
    static int TypeID ;
    enum PeerStatus {Busy,Free};
    Q_ENUMS(PeerStatus)
    PeerViewInfoMsg(const QString& name = "", int port = 0, PeerStatus status = Free, QObject* p = 0);

    MetaPropertyPublicSet_Ex(QString, name)
    MetaPropertyPublicSet_Ex(int, port)
    MetaPropertyPublicSet_Ex(PeerStatus, status)

    virtual void read(QDataStream &dataBuffer);
    virtual void write(QDataStream &dataBuffer);
    virtual int typeId();
};
