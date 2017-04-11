#pragma once

#include "JObject.h"
#include <QUrl>
#include <QFileInfoList>

#define FileMgr FileTransferManager::me()

class Connection;
class Message;
class FileTransferMsg;


class FileSenderHandler : public JObject
{
    Q_OBJECT
public:
    FileSenderHandler(Connection* conn, const QStringList& files, QObject* p = 0);

signals:
    void startingFile(const QString& file);
    void finished();

private slots:
    void startSending();
    void sendRootFile();
    void onNewMsgCame(Connection *sender, Message *msg);

private:
    void parseFile(const QFileInfo& file);
    void sendFile();
    void sendFilePart(int seqNo);

private:
    QStringList mRootFiles;
    int mCurrentRootFileIndex;
    QFileInfoList mAllFiles;
    int mCurrentFileIndex;
    Connection* mConnection;

    // a root transfer related;
    QString mCurrentUUID;
    QString mCurrentBasePath;

    // a file transfer related
    int mTotalSeqCount;
    QFile* mFile;
};

class FileReceiverHandler : public JObject {
    Q_OBJECT
public:
    FileReceiverHandler(Connection* conn, FileTransferMsg* msg, QObject* p = 0);

signals:
    void finished();

private slots:
    void startReceiving();
    void onNewMsgCame(Connection *sender, Message *msg);

private:
    Connection* mConnection;
    FileTransferMsg* mFileMsg;
    QFile* mFile;
};


struct FileTransferManagerPri;

class FileTransferManager : public JObject
{
    Q_OBJECT
    FileTransferManager(QObject* p = 0);
public:
    static FileTransferManager* me();

public slots:
    void shareFilesTo(Connection* conn, const QList<QUrl> &urls);

private slots:
    void onNewMsgCame(Connection *sender, Message *msg);

private:
    FileTransferManagerPri* d;
};
