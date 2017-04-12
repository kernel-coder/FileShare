#include "FilePartTransferMsg.h"


FilePartTransferAckMsg::FilePartTransferAckMsg(const QString& rootUuid, const QString &uuid, int seqNo, QObject *p)
    : Message(p)
    , _rootUuid(rootUuid)
    , _uuid(uuid)
    , _seqNo(seqNo)
{

}


void FilePartTransferAckMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << _rootUuid;
    buf << _uuid;
    buf << _seqNo;
}


void FilePartTransferAckMsg::read(QDataStream &buf)
{
    buf >> _rootUuid;
    buf >> _uuid;
    buf >> _seqNo;
}


int FilePartTransferAckMsg::typeId() { return FilePartTransferAckMsg::TypeID;}




FilePartTransferMsg::FilePartTransferMsg(const QString& rootUuid, const QString &uuid, int seqNo,
                                         const QByteArray &data, QObject *p)
    : FilePartTransferAckMsg(rootUuid, uuid, seqNo, p)
    , _data(data)
{

}


void FilePartTransferMsg::write(QDataStream &buf)
{
    FilePartTransferAckMsg::write(buf);
    buf << _data.length();
    buf.writeRawData(_data.data(), _data.length());
}


void FilePartTransferMsg::read(QDataStream &buf)
{
    FilePartTransferAckMsg::read(buf);
    int length  =0;
    buf >> length;
    QByteArray ba(length, 0);
    buf.readRawData(ba.data(), length);
    _data = ba;
}


int FilePartTransferMsg::typeId() { return FilePartTransferMsg::TypeID;}



