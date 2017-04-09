#ifndef PEERVIEWINFOMSG_H
#define PEERVIEWINFOMSG_H

#include "Message.h"
#include <QColor>

class PeerViewInfoMsg : public Message
{
    Q_OBJECT
public:
    PeerViewInfoMsg();
    PeerViewInfoMsg(QString name);
    ~PeerViewInfoMsg();

    void setName(const QString name);
    QString name()const;

    virtual void read(QDataStream &dataBuffer);
    virtual void write(QDataStream &dataBuffer);
    virtual int typeId();

public:
    static int TypeID ;

private:
    QString mName;
};

#endif // PEERVIEWINFOMSG_H
