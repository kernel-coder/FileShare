#include "FileTransferMsg.h"
#include "Utils.h"
#include <QFileInfo>
#include <QFile>
#include <QDebug>


FileTransferHeaderInfoMsg::FileTransferHeaderInfoMsg(const QString& transferId,
                                                     const QString& filePath,
                                                     int count, int index,
                                                     quint64 sizeTotal, quint64 sizeProgress, QObject* p)
    : Message(p)
    , _transferId(transferId)
    , _filePath(filePath)
    , _fileCount(count)
    , _fileIndex(index)
    , _totalSize(sizeTotal)
    , _progressSize(sizeProgress)
{

}


void FileTransferHeaderInfoMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << _transferId;
    buf << _filePath;
    buf << _fileCount;
    buf << _fileIndex;
    buf << _totalSize;
    buf << _progressSize;
}


void FileTransferHeaderInfoMsg::read(QDataStream &buf)
{
    buf >> _transferId;
    buf >> _filePath;
    buf >> _fileCount;
    buf >> _fileIndex;
    buf >> _totalSize;
    buf >> _progressSize;
}


int FileTransferHeaderInfoMsg::typeId() { return FileTransferHeaderInfoMsg::TypeID;}


FileTransferMsg::FileTransferMsg(const QString& transferId, const QString& uuid, const QString& filename, quint64 startPos, QObject* p)
    : Message(p)
    , _transferId(transferId)
    , _uuid(uuid)
    , _filename(filename)
    , _startPos(startPos)
{

}


int FileTransferMsg::typeId() { return FileTransferMsg::TypeID;}


void FileTransferMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << _transferId;
    buf << _uuid;
    buf << _basePath;
    buf << _filename;
    buf << _fileNo;
    buf << _size;
    buf << _seqCount;
    buf << _startPos;
    buf << _progressSize;
}


void FileTransferMsg::read(QDataStream &buf)
{
    buf >> _transferId;
    buf >> _uuid;
    buf >> _basePath;
    buf >> _filename;
    buf >> _fileNo;
    buf >> _size;
    buf >> _seqCount;
    buf >> _startPos;
    buf >> _progressSize;
}


FileTransferAckMsg::FileTransferAckMsg(const QString& transferId,
                                       const QString& uuid,
                                       const QString& filename,
                                       bool skipping,
                                       quint64 startPos, QObject* p)
    : FileTransferMsg(transferId, uuid, filename, startPos, p)
    , _skipping(skipping)
{
}


int FileTransferAckMsg::typeId() { return FileTransferAckMsg::TypeID;}

void FileTransferAckMsg::write(QDataStream &buf)
{
    FileTransferMsg::write(buf);
    buf << _skipping;
}


void FileTransferAckMsg::read(QDataStream &buf)
{
    FileTransferMsg::read(buf);
    buf >> _skipping;
}
