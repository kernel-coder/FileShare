#ifndef PEERVIEWINFOMSG_H
#define PEERVIEWINFOMSG_H

#include "Message.h"
#include <QColor>

class PeerViewInfoMsg : public Message
{
    Q_OBJECT
public:
    PeerViewInfoMsg();
    PeerViewInfoMsg(QString name,QColor color);
    ~PeerViewInfoMsg();

    void setName(const QString name);
    QString name()const;
    void setColor(const QColor color);
    QColor color()const;

    virtual void read(QDataStream &dataBuffer);
    virtual void write(QDataStream &dataBuffer);
    virtual int typeId();

public:
    static int TypeID ;

private:
    QString mName;
    QColor mColor;
};

#endif // PEERVIEWINFOMSG_H
