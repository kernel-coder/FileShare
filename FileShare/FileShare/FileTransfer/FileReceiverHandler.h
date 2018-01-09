#pragma once

#include "FileHandlerBase.h"
#include <QUrl>
#include <QFileInfoList>
#include <QThread>

class FileReceiverHandler : public FileHandlerBase
{
    Q_OBJECT
public:
    FileReceiverHandler(Connection* conn, FileTransferHeaderInfoMsg* msg, QObject* p = 0);
    ~FileReceiverHandler();

protected:
    void handleInitialize();
    void handleMessageComingFrom(Connection* conn, Message* msg);

private:
    FileTransferHeaderInfoMsg* mHeaderInfoMsg;
    QFile* mFile;
    int mFileFlushMark;
    QString mFileUUID;
    qint64 mFileSeqCount;
};
