#include "FilePartTransferMsg.h"


FilePartTransferAckMsg::FilePartTransferAckMsg(const QString& transferId, const QString &uuid,
                                               int fileNo, qint64 seqNo, int size, quint64 progressSize, QObject *p)
    : Message(p)
    , _transferId(transferId)
    , _uuid(uuid)
    , _fileNo(fileNo)
    , _seqNo(seqNo)
    , _size(size)
    , _progressSize(progressSize)
{

}


void FilePartTransferAckMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << _transferId;
    buf << _uuid;
    buf << _fileNo;
    buf << _seqNo;
    buf << _size;
    buf << _progressSize;
}


void FilePartTransferAckMsg::read(QDataStream &buf)
{
    buf >> _transferId;
    buf >> _uuid;
    buf >> _fileNo;
    buf >> _seqNo;
    buf >> _size;
    buf >> _progressSize;
}


int FilePartTransferAckMsg::typeId() { return FilePartTransferAckMsg::TypeID;}




FilePartTransferMsg::FilePartTransferMsg(const QString& transferId, const QString &uuid, int fileNo,
                                         qint64 seqNo, int size, quint64 progressSize, const QByteArray &data, QObject *p)
    : FilePartTransferAckMsg(transferId, uuid, fileNo, seqNo, size, progressSize, p)
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



