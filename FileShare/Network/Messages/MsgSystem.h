#ifndef MSGSYSTEM_H
#define MSGSYSTEM_H

#include <QObject>
#include <QDataStream>
#include "PeerViewInfoMsg.h"
class Message;
class QDataStream;

class MsgSystem : public QObject
{
Q_OBJECT
public:
    explicit MsgSystem(QObject *parent = 0);
    static Message *readAndContruct(QDataStream &pStream);
    static  QString readableStatus(PeerViewInfoMsg::PeerStatus status);

signals:

public slots:

};

#endif // MSGSYSTEM_H
