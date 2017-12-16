#pragma once

#include "JObject.h"
#include <QUrl>
#include <QFileInfoList>
#include <QThread>
#include "Messages/TransferControlMsg.h"
#include <QUrl>
#include <QFileInfoList>

#define FileMgr FileTransferManager::me()

class Connection;
class Message;
class FileTransferHeaderInfoMsg;
class FileTransferMsg;
class FileTransferAckMsg;
class FilePartTransferAckMsg;
class FileSenderHandler;
class FileReceiverHandler;
class FileHandlerBase;


class RootFileUIInfo : public JObject {
    Q_OBJECT
public:
    Q_INVOKABLE explicit RootFileUIInfo(QObject* p = 0);
    MetaPropertyPublicSet(QString, transferId)
    MetaPropertyPublicSet(bool, isSending)
    MetaPropertyPublicSet(QString, filePathRoot)
    MetaPropertyPublicSet(QString, filePath)
    MetaPropertyPublicSet(int, countTotalFile)
    MetaPropertyPublicSet(int, countFileProgress)
    MetaPropertyPublicSet(quint64, sizeTotalFile)
    MetaPropertyPublicSet(quint64, sizeFileProgress)
    MetaPropertyPublicSet(TransferStatusFlag::ControlStatus, transferStatus)
};


class UITransferInfoItem: public JObject {
    Q_OBJECT
public:
    static UITransferInfoItem* create(RootFileUIInfo* rootFileInfo);
    static UITransferInfoItem* create(const QString& chatMsg, bool sending);
    Q_INVOKABLE explicit UITransferInfoItem(QObject* p = 0);
    MetaPropertyPublicSet(QString, itemId)
    MetaPropertyPublicSet_Ptr(RootFileUIInfo, fileInfo)
    MetaPropertyPublicSet(bool, isFileTransfer)
    MetaPropertyPublicSet(QString, chatMsg)
    MetaPropertyPublicSet(bool, isChatSending)
};


class TransferFailedItem : public JObject {
    Q_OBJECT
public:
    Q_INVOKABLE TransferFailedItem(QObject* p = 0) : JObject(p) {}
    MetaPropertyPublicSet(QString, transferId)
    MetaPropertyPublicSet_List(QString, rootFiles)
    MetaPropertyPublicSet(int, rootFileIndex)
    MetaPropertyPublicSet(int, fileIndex)
    MetaPropertyPublicSet(int, seqIndex)
    MetaPropertyPublicSet(quint64, progressSize)
};

class TransferFailedItems : public JObject {
    Q_OBJECT
public:
    TransferFailedItems(QObject* p = 0) : JObject(p) {}
    MetaPropertyPublicSet_Ptr_List(TransferFailedItem,  failedTransfers)
};



struct FileTransferManagerPrivate;
class FileTransferManager : public QObject {
    Q_OBJECT
    FileTransferManager(QObject* p = 0);
public:
    static FileTransferManager* me();
    ~FileTransferManager();

    void addSenderHandler(Connection* conn, FileSenderHandler* fsh);
    void addReceiverHandler(Connection* conn, FileReceiverHandler* frh, FileTransferHeaderInfoMsg* msg);
    QString saveFolderPathForTransferID(Connection* conn, const QString& transferId);

    bool resumeFailedTransfer(Connection *conn, const QString& transferId);
    void addFailedTransferItem(TransferFailedItem* item);
    TransferFailedItem* removeFailedTransferItem(const QString& transferId);

signals:
    void fileTransfer(Connection* conn, UITransferInfoItem* uiInfo);
    void chatTransfer(Connection* conn, UITransferInfoItem* uiInfo);

public slots:
    void shareFilesTo(Connection* conn, const QList<QUrl> &urls);
    void sendChatTo(Connection* conn, const QString& msg);
    void applyControlStatus(Connection* conn, RootFileUIInfo* item, int status);
    void deleteItem(Connection* conn, const QString& itemId);

private slots:
    void onNewMsgCome(Connection *sender, Message *msg);
    void onSendingRootFile(Connection* conn, FileTransferHeaderInfoMsg* msg, const QString& sourcePath);
    void onFilePartSent(Connection* conn, FilePartTransferAckMsg* msg);
    void onReceivedFilePart(Connection* conn, FilePartTransferAckMsg* msg);

private:
    FileTransferManagerPrivate* d;
};
