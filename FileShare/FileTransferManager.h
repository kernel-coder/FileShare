#pragma once

#include "JObject.h"
#include <QUrl>
#include <QFileInfoList>
#include <QThread>

#define FileMgr FileTransferManager::me()

class Connection;
class Message;
class FileTransferMsg;


class FileHandler : public QThread
{
    Q_OBJECT
public:
    FileHandler(Connection* conn, QObject* p = 0);
signals:
    void sendMsg(Message* msg);

private slots:
    void onThreadStarted();
    void onMessageComeFrom(Connection* conn, Message* msg);

protected:
    virtual void handleThreadStarting() = 0;
    virtual void handleMessageComingFrom(Connection* conn, Message* msg) = 0;

protected:
    Connection* mConnection;
};

class FileSenderHandler : public FileHandler
{
    Q_OBJECT
public:
    FileSenderHandler(Connection* conn, const QStringList& files, QObject* p = 0);

protected:
    void handleThreadStarting();
    void handleMessageComingFrom(Connection* conn, Message* msg);

signals:
    void startingFile(const QString& file);

private slots:
    void sendRootFile();

private:
    void parseFile(const QFileInfo& file);
    void sendFile();
    void sendFilePart(int seqNo);

private:
    QStringList mRootFiles;
    int mCurrentRootFileIndex;
    QFileInfoList mAllFiles;
    int mCurrentFileIndex;

    // a root transfer related;
    QString mCurrentUUID;
    int mIndexOfBasePath;

    // a file transfer related
    int mTotalSeqCount;
    QFile* mFile;
};

class FileReceiverHandler : public FileHandler
{
    Q_OBJECT
public:
    FileReceiverHandler(Connection* conn, FileTransferMsg* msg, QObject* p = 0);

protected:
    void handleThreadStarting();
    void handleMessageComingFrom(Connection* conn, Message* msg);


private:
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
    void onNewMsgCome(Connection *sender, Message *msg);

private:
    FileTransferManagerPri* d;
};
