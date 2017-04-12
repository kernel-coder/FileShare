#pragma once

#include "Message.h"

class FilePartTransferAckMsg : public Message
{
    Q_OBJECT
public:
    static int TypeID ;
    FilePartTransferAckMsg(const QString& rootUuid = "", const QString& uuid = "", int fileNo = -1, int seqNo = -1,
                           int size  = 0, QObject* p =0);

    MetaPropertyPublicSet_Ex(QString, rootUuid)
    MetaPropertyPublicSet_Ex(QString, uuid)
    MetaPropertyPublicSet_Ex(int, seqNo)
    MetaPropertyPublicSet_Ex(int, fileNo)
    MetaPropertyPublicSet_Ex(int, size)

    virtual void read(QDataStream &buf);
    virtual void write(QDataStream &buf);
    virtual int typeId();
};

class FilePartTransferMsg : public FilePartTransferAckMsg
{
    Q_OBJECT
public:
    static int TypeID ;
    FilePartTransferMsg(const QString& rootUuid = "", const QString& uuid = "", int fileNo = -1,
                        int seqNo = -1, int size  = 0,
                        const QByteArray& data = QByteArray(), QObject* p =0);

    MetaPropertyPublicSet_Ex(QByteArray, data)

    virtual void read(QDataStream &buf);
    virtual void write(QDataStream &buf);
    virtual int typeId();
};



