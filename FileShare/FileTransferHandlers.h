#pragma once

#include "JObject.h"
#include <QUrl>
#include <QFileInfoList>
#include <QThread>

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

    // a root transfer related
    QString mRootUuid;
    QFileInfoList mAllFiles;
    int mCurrentFileIndex;
    int mIndexOfBasePath;
    quint64 mRootTotalSize;

    // a file transfer related
    QString mFileUuid;
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


class FileTransferUIInfoHandler : public QObject {
    Q_OBJECT
    FileTransferUIInfoHandler(QObject* p = 0);

public:
    static FileTransferUIInfoHandler* me();

};
