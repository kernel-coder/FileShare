#include "FileTransferMsg.h"
#include "Utils.h"
#include <QFileInfo>
#include <QFile>
#include <QDebug>


FileTransferMsg::FileTransferMsg(const QString& uuid, const QString& filename, QObject* p)
    : Message(p)
    , _uuid(uuid)
    , _filename(filename)
{

}


void FileTransferMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << _uuid;
    buf << _basePath;
    buf << _filename;
    buf << _size;
    buf << _seqCount;
}


void FileTransferMsg::read(QDataStream &buf)
{
    buf >> _uuid;
    buf >> _basePath;
    buf >> _filename;
    buf >> _size;
    buf >> _seqCount;
}


int FileTransferMsg::typeId() { return FileTransferMsg::TypeID;}


FileTransferAckMsg::FileTransferAckMsg(const QString& uuid, const QString& filename, QObject* p)
    : FileTransferMsg(uuid, filename)
{
}


int FileTransferAckMsg::typeId() { return FileTransferMsg::TypeID;}
