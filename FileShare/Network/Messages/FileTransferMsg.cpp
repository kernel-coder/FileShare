#include "FileTransferMsg.h"
#include "Utils.h"
#include <QFileInfo>
#include <QFile>


FileTransferMsg::FileTransferMsg(const QString& filename, QObject* p)
    : Message(p)
    , _filename(filename)
{

}


void FileTransferMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << QFileInfo(_filename).fileName();
    QFile file(_filename);
    if (file.open(QFile::ReadOnly)) {
        QByteArray ba = file.readAll();
        buf << ba.length();
        buf.writeRawData(ba.data(), ba.length());
        file.close();
    }
    else {
        buf << ((qint64)0);
    }
}


void FileTransferMsg::read(QDataStream &buf)
{
    QString filename;
    int length  =0;
    buf >> filename;
    buf >> length;
    QByteArray ba;
    ba.resize(length);
    buf.readRawData(ba.data(), length);
    _filename = Utils::me()->dataDirCommon(filename);
    QFile file(_filename);
    if (file.open(QFile::WriteOnly)) {
        file.write(ba);
        file.close();
    }
}


int FileTransferMsg::typeId() { return FileTransferMsg::TypeID;}

