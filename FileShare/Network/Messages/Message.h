#pragma once

#include "JObject.h"
#include <QMetaType>
#include <QDataStream>

class Message: public JObject
{
    Q_OBJECT
public:
    Message(QObject* p = 0);
    virtual ~Message();

    virtual void read(QDataStream &dataBuffer) = 0;
    virtual void write(QDataStream &dataBuffer) = 0;
    virtual int typeId() = 0;
};

