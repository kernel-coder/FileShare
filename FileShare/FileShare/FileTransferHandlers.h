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
    const int MSG_LEN = 4*1024*1024;
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
    ~FileReceiverHandler();

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
    Q_INVOKABLE explicit RootFileUIInfo(QObject* p = 0) : JObject(p) {}
    MetaPropertyPublicSet(bool, isSending)
    MetaPropertyPublicSet(QString, filePathRoot)
    MetaPropertyPublicSet(QString, filePath)
    MetaPropertyPublicSet(int, countTotalFile)
    MetaPropertyPublicSet(int, countFileProgress)
    MetaPropertyPublicSet(quint64, sizeTotalFile)
    MetaPropertyPublicSet(quint64, sizeFileProgress)
};


class UITransferInfoItem: public JObject {
    Q_OBJECT
public:
    static UITransferInfoItem* create(QObject* parent, RootFileUIInfo* rootFileInfo);
    static UITransferInfoItem* create(QObject* parent, const QString& chatMsg, bool sending);
    Q_INVOKABLE explicit UITransferInfoItem(QObject* p = 0) : JObject(p) {}
    MetaPropertyPublicSet_Ptr(RootFileUIInfo, fileInfo)
    MetaPropertyPublicSet(bool, isFileTransfer)
    MetaPropertyPublicSet(QString, chatMsg)
    MetaPropertyPublicSet(bool, isChatSending)
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
    void fileTransfer(Connection* conn, UITransferInfoItem* uiInfo);

private slots:
    void onSendingRootFile(Connection* conn, FileTransferHeaderInfoMsg* msg, const QString& sourcePath);
    void onFileSent(Connection* conn, FileTransferAckMsg* msg);
    void onFilePartSent(Connection* conn, FilePartTransferAckMsg* msg);
    void onReceivedFilePart(Connection* conn, FilePartTransferAckMsg* msg);

private:
    FileTransferUIInfoHandlerPrivate* d;
};
