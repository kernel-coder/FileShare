#include "FileTransferMsg.h"
#include "Utils.h"
#include <QFileInfo>
#include <QFile>
#include <QDebug>


FileTransferHeaderInfoMsg::FileTransferHeaderInfoMsg(const QString& rootUuid, const QString& filePath, int count, quint64 size, QObject* p)
    : Message(p)
    , _rootUuid(rootUuid)
    , _filePath(filePath)
    , _fileCount(count)
    , _totalSize(size)
{

}


void FileTransferHeaderInfoMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << _rootUuid;
    buf << _filePath;
    buf << _fileCount;
    buf << _totalSize;
}


void FileTransferHeaderInfoMsg::read(QDataStream &buf)
{
    buf >> _rootUuid;
    buf >> _filePath;
    buf >> _fileCount;
    buf >> _totalSize;
}


int FileTransferHeaderInfoMsg::typeId() { return FileTransferHeaderInfoMsg::TypeID;}


FileTransferMsg::FileTransferMsg(const QString& rootUuid, const QString& uuid, const QString& filename, QObject* p)
    : Message(p)
    , _rootUuid(rootUuid)
    , _uuid(uuid)
    , _filename(filename)
{

}


void FileTransferMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << _rootUuid;
    buf << _uuid;
    buf << _basePath;
    buf << _filename;
    buf << _size;
    buf << _seqCount;
}


void FileTransferMsg::read(QDataStream &buf)
{
    buf >> _rootUuid;
    buf >> _uuid;
    buf >> _basePath;
    buf >> _filename;
    buf >> _size;
    buf >> _seqCount;
}


int FileTransferMsg::typeId() { return FileTransferMsg::TypeID;}


FileTransferAckMsg::FileTransferAckMsg(const QString& rootUuid, const QString& uuid, const QString& filename, QObject* p)
    : FileTransferMsg(rootUuid, uuid, filename, p)
{
}


int FileTransferAckMsg::typeId() { return FileTransferAckMsg::TypeID;}
