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

signals:
    void receivedFilePart(Connection* conn, FilePartTransferAckMsg* msg);

protected:
    void handleThreadStarting();
    void handleMessageComingFrom(Connection* conn, Message* msg);

private:
    FileTransferHeaderInfoMsg* mHeaderInfoMsg;
    FileTransferMsg* mFileMsg;
    QFile* mFile;
    int mFileFlushMark;
};
