#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QMetaType>
#include <QDataStream>

class Message: public QObject
{
    Q_OBJECT
public:
    Message();
    virtual ~Message();

    virtual void read(QDataStream &dataBuffer) = 0;
    virtual void write(QDataStream &dataBuffer) = 0;
    virtual int typeId() = 0;
};

#endif // MESSAGE_H
