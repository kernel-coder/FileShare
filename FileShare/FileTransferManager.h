#pragma once

#include "JObject.h"
#include <QUrl>

#define FTM FileTransferManager()::me()

class Connection;

class FileTransferManager : public JObject
{
    Q_OBJECT
    FileTransferManager(QObject* p = 0);
public:
    static FileTransferManager* me();


public slots:
    void shareFilesTo(Connection* conn, const QList<QUrl> &urls);
};
