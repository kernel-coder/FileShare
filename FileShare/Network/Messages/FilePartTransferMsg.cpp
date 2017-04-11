#include "FilePartTransferMsg.h"

FilePartTransferMsg::FilePartTransferMsg(const QString &uuid, int seqNo,
                                         const QByteArray &data, QObject *p)
    : Message(p)
    , _uuid(uuid)
    , _seqNo(seqNo)
    , _data(data)
{

}


void FilePartTransferMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << _uuid;
    buf << _seqNo;
    buf << _data.length();
    buf.writeRawData(_data.data(), _data.length());
}


void FilePartTransferMsg::read(QDataStream &buf)
{
    buf << _uuid;
    buf << _seqNo;
    int length  =0;
    buf << length;
    QByteArray ba;
    ba.resize(length);
    buf.readRawData(ba.data(), length);
}


int FilePartTransferMsg::typeId() { return FilePartTransferMsg::TypeID;}


FilePartTransferAckMsg::FilePartTransferAckMsg(const QString &uuid, int seqNo, QObject *p)
    : Message(p)
    , _uuid(uuid)
    , _seqNo(seqNo)
{

}


void FilePartTransferAckMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << _uuid;
    buf << _seqNo;
}


void FilePartTransferAckMsg::read(QDataStream &buf)
{
    buf << _uuid;
    buf << _seqNo;
}


int FilePartTransferAckMsg::typeId() { return FilePartTransferAckMsg::TypeID;}

