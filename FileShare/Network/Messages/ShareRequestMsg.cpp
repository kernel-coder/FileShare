#include "ShareRequestMsg.h"

ShareRequestMsg::ShareRequestMsg(const QStringList& files , QObject* p)
    : Message(p)
    , _files(files)
{

}


void ShareRequestMsg::write(QDataStream &dataBuffer)
{
    dataBuffer << typeId();
    dataBuffer << _files.length();
    foreach (QString file, _files) {
        dataBuffer << file;
    }
}


void ShareRequestMsg::read(QDataStream &dataBuffer)
{
    int length;
    dataBuffer >> length;
    for (int i = 0; i < length; i++) {
        QString file;
        dataBuffer >> file;
        _files.append(file);
    }
}


int ShareRequestMsg::typeId() { return ShareRequestMsg::TypeID;}

