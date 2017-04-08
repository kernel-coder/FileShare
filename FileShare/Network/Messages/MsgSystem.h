#ifndef MSGSYSTEM_H
#define MSGSYSTEM_H

#include <QObject>
#include <QDataStream>
#include "ServerInfoMsg.h"
class Message;
class QDataStream;

class MsgSystem : public QObject
{
Q_OBJECT
public:
    explicit MsgSystem(QObject *parent = 0);
    static Message *readAndContruct(QDataStream &pStream);
    static  QString readableStatus(ServerInfoMsg::MyStatus status);

signals:

public slots:

};

#endif // MSGSYSTEM_H
