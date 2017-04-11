#include "ShareRequestMsg.h"

ShareRequestMsg::ShareRequestMsg(int requestId, const QStringList& files , QObject* p)
    : Message(p)
    , _requestId(requestId)
    , _files(files)
{

}


void ShareRequestMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << _requestId;
    buf << _files.length();
    foreach (QString file, _files) {
        buf << file;
    }
}


void ShareRequestMsg::read(QDataStream &buf)
{
    int length;
    buf >> _requestId;
    buf >> length;
    for (int i = 0; i < length; i++) {
        QString file;
        buf >> file;
        _files.append(file);
    }
}


int ShareRequestMsg::typeId() { return ShareRequestMsg::TypeID;}

