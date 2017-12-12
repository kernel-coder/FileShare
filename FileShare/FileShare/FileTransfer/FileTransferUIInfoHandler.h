#pragma once

#include "JObject.h"
#include <QUrl>
#include <QFileInfoList>
#include <QThread>
#include "Messages/TransferControlMsg.h"

#define FileMgrUIHandler FileTransferUIInfoHandler::me()

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
    Q_INVOKABLE explicit RootFileUIInfo(QObject* p = 0) : JObject(p) {}
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
    Q_INVOKABLE explicit UITransferInfoItem(QObject* p = 0) : JObject(p) {}
    MetaPropertyPublicSet(QString, itemId)
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

    FileHandlerBase* getHandler(const QString& transferId);
    void addSenderHandler(Connection* conn, FileSenderHandler* fsh);
    void addReceiverHandler(Connection* conn, FileReceiverHandler* frh, FileTransferHeaderInfoMsg* msg);
    QString saveFolderPathForTransferID(Connection* conn, const QString& transferId);

signals:
    void fileTransfer(Connection* conn, UITransferInfoItem* uiInfo);

public slots:
    void applyControlStatus(Connection* conn, RootFileUIInfo* item, int status);
    void deleteItem(Connection* conn, const QString& itemId);

private slots:
    void onSendingRootFile(Connection* conn, FileTransferHeaderInfoMsg* msg, const QString& sourcePath);
    void onFileSent(Connection* conn, FileTransferAckMsg* msg);
    void onFilePartSent(Connection* conn, FilePartTransferAckMsg* msg);
    void onReceivedFilePart(Connection* conn, FilePartTransferAckMsg* msg);

private:
    FileTransferUIInfoHandlerPrivate* d;
};
