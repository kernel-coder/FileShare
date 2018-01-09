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
    void cleanup(TransferStatusFlag::ControlStatus);
    void handleInitialize();
    void handleMessageComingFrom(Connection* conn, Message* msg);

signals:
    void startingFile(Connection* conn, const QString& file);
    void sendingRootFile(Connection* conn, FileTransferHeaderInfoMsg* msg, const QString& sourcePath);    

private slots:
    void sendRootFile();

private:
    void parseFile(const QFileInfo& file);
    void sendFile();
    void sendFilePart(qint64 seqNo);

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
    qint64 mTotalSeqCount;
    QFile* mFile;
    qint64 mCurrentSeqNo;
};
