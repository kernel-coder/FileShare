#pragma once

#include "FileHandlerBase.h"
#include <QUrl>
#include <QFileInfoList>

class TransferFailedItem;


class FileSenderHandler : public FileHandlerBase
{
    Q_OBJECT
public:
    FileSenderHandler(Connection* conn, const QStringList& files, QObject* p = 0);
    FileSenderHandler(Connection *conn, TransferFailedItem* item, QObject *p = 0);

protected:
    void cleanup(bool success);
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
    bool mTransferDone;
    TransferFailedItem* mFailedItem;
    QStringList mRootFiles;
    int mCurrentRootFileIndex;

    // a root transfer related
    QFileInfoList mAllFiles;
    int mCurrentFileIndex;
    int mIndexOfBasePath;
    quint64 mRootTotalSize;
    quint64 mRootProgressSize;

    // a file transfer related
    QString mFileUuid;
    int mTotalSeqCount;
    QFile* mFile;
    int mCurrentSeqNo;

};
