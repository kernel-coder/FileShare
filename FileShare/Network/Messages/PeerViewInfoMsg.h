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
    PeerViewInfoMsg(const QString& name = "", int port = 0, PeerStatus status = Free,
                    const QString& deviceId = "", QObject* p = 0);

    MetaPropertyPublicSet_Ex(QString, name)
    MetaPropertyPublicSet_Ex(int, port)
    MetaPropertyPublicSet_Ex(PeerStatus, status)
    MetaPropertyPublicSet_Ex(QString, deviceId)

    virtual void read(QDataStream &buf);
    virtual void write(QDataStream &buf);
    virtual int typeId();
};
