#pragma once

#include "Message.h"

class FileTransferHeaderInfoMsg : public Message
{
    Q_OBJECT
public:
    static int TypeID ;
    FileTransferHeaderInfoMsg(const QString& transferId = ""
            , const QString& filePath = ""
            , int count = 0
            , quint64 size = 0
            , QObject* p = 0);

    MetaPropertyPublicSet_Ex(QString, transferId)
    MetaPropertyPublicSet_Ex(QString, filePath)
    MetaPropertyPublicSet_Ex(int, fileCount)
    MetaPropertyPublicSet_Ex(quint64, totalSize)

    virtual void read(QDataStream &buf);
    virtual void write(QDataStream &buf);
    virtual int typeId();
};

class FileTransferHeaderInfoAckMsg : public FileTransferHeaderInfoMsg
{
    Q_OBJECT
public:
    static int TypeID ;
    FileTransferHeaderInfoAckMsg(const QString& transferId = ""
            , const QString& filePath = ""
            , int count = 0
            , quint64 size = 0
            , QObject* p = 0)
        : FileTransferHeaderInfoMsg(transferId, filePath, count, size, p) {}
    virtual int typeId() {return FileTransferHeaderInfoAckMsg::TypeID;}
};


class FileTransferMsg : public Message
{
    Q_OBJECT
public:
    static int TypeID ;
    FileTransferMsg(const QString& transferId = ""
            , const QString& uuid = ""
            , const QString& filename = ""
            , QObject* p = 0);

    MetaPropertyPublicSet_Ex(QString, transferId)
    MetaPropertyPublicSet_Ex(QString, uuid)
    MetaPropertyPublicSet_Ex(QString, basePath)
    MetaPropertyPublicSet_Ex(QString, filename)
    MetaPropertyPublicSet_Ex(int, fileNo)
    MetaPropertyPublicSet_Ex(qint64, size)
    MetaPropertyPublicSet_Ex(int, seqCount)

    virtual void read(QDataStream &buf);
    virtual void write(QDataStream &buf);
    virtual int typeId();
};


class FileTransferAckMsg : public FileTransferMsg
{
    Q_OBJECT
public:
    static int TypeID ;
    FileTransferAckMsg(const QString& transferId = ""
            , const QString& uuid = ""
            , const QString& filename = ""
            , QObject* p = 0);

    virtual int typeId();
};
