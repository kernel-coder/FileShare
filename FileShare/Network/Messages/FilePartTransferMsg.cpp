#include "FilePartTransferMsg.h"


FilePartTransferAckMsg::FilePartTransferAckMsg(const QString& rootUuid, const QString &uuid,
                                               int fileNo, int seqNo, int size, QObject *p)
    : Message(p)
    , _rootUuid(rootUuid)
    , _uuid(uuid)
    , _fileNo(fileNo)
    , _seqNo(seqNo)
    , _size(size)
{

}


void FilePartTransferAckMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << _rootUuid;
    buf << _uuid;
    buf << _fileNo;
    buf << _seqNo;
    buf << _size;
}


void FilePartTransferAckMsg::read(QDataStream &buf)
{
    buf >> _rootUuid;
    buf >> _uuid;
    buf >> _fileNo;
    buf >> _seqNo;
    buf >> _size;
}


int FilePartTransferAckMsg::typeId() { return FilePartTransferAckMsg::TypeID;}




FilePartTransferMsg::FilePartTransferMsg(const QString& rootUuid, const QString &uuid, int fileNo,
                                         int seqNo, int size, const QByteArray &data, QObject *p)
    : FilePartTransferAckMsg(rootUuid, uuid, fileNo, seqNo, size, p)
    , _data(data)
{

}


void FilePartTransferMsg::write(QDataStream &buf)
{
    FilePartTransferAckMsg::write(buf);
    buf.writeRawData(_data.data(), size());
}


void FilePartTransferMsg::read(QDataStream &buf)
{
    FilePartTransferAckMsg::read(buf);
    QByteArray ba(size(), 0);
    buf.readRawData(ba.data(), size());
    _data = ba;
}


int FilePartTransferMsg::typeId() { return FilePartTransferMsg::TypeID;}



