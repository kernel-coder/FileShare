#pragma once

#include "JObject.h"
#include <QUrl>
#include <QFileInfoList>
#include <QThread>

#define FileMgrUIHandler FileTransferUIInfoHandler::me()

class Connection;
class Message;
class FileTransferHeaderInfoMsg;
class FileTransferMsg;
class FileTransferAckMsg;
class FilePartTransferAckMsg;



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
    void startingFile(Connection* conn, const QString& file);
    void sendingRootFile(Connection* conn, FileTransferHeaderInfoMsg* msg, const QString& sourcePath);
    void fileSent(Connection* conn, FileTransferAckMsg* msg);
    void filePartSent(Connection* conn, FilePartTransferAckMsg* msg);

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

signals:
    void receivedFilePart(Connection* conn, FilePartTransferAckMsg* msg);

protected:
    void handleThreadStarting();
    void handleMessageComingFrom(Connection* conn, Message* msg);

private:
    FileTransferMsg* mFileMsg;
    QFile* mFile;
};



class RootFileUIInfo : public JObject {
    Q_OBJECT
public:
    RootFileUIInfo(QObject* p = 0) : JObject(p) {}
    MetaPropertyPublicSet_Ex(bool, isSending)
    MetaPropertyPublicSet_Ex(QString, filePathRoot)
    MetaPropertyPublicSet_Ex(QString, filePath)
    MetaPropertyPublicSet_Ex(int, countTotalFile)
    MetaPropertyPublicSet_Ex(int, countFileProgress)
    MetaPropertyPublicSet_Ex(quint64, sizeTotalFile)
    MetaPropertyPublicSet_Ex(quint64, sizeFileProgress)
};



struct FileTransferUIInfoHandlerPrivate;
class FileTransferUIInfoHandler : public QObject {
    Q_OBJECT
    FileTransferUIInfoHandler(QObject* p = 0);
public:
    static FileTransferUIInfoHandler* me();
    ~FileTransferUIInfoHandler();

    void addSenderHandler(Connection* conn, FileSenderHandler* fsh);
    void addRootFileReceiverHandler(Connection* conn, FileTransferHeaderInfoMsg* msg);
    void addReceiverHandler(Connection* conn, FileReceiverHandler* frh);
    QString saveFolderPathForRootUUID(const QString& rootUuid);

signals:
    void fileTransfer(Connection* conn, RootFileUIInfo* uiInfo);

private slots:
    void onSendingRootFile(Connection* conn, FileTransferHeaderInfoMsg* msg, const QString& sourcePath);
    void onFileSent(Connection* conn, FileTransferAckMsg* msg);
    void onFilePartSent(Connection* conn, FilePartTransferAckMsg* msg);
    void onReceivedFilePart(Connection* conn, FilePartTransferAckMsg* msg);

private:
    FileTransferUIInfoHandlerPrivate* d;
};
