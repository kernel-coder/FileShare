#pragma once

#include "FileHandlerBase.h"
#include <QUrl>
#include <QFileInfoList>


class FileSenderHandler : public FileHandlerBase
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
    QFileInfoList mAllFiles;
    int mCurrentFileIndex;
    int mIndexOfBasePath;
    quint64 mRootTotalSize;

    // a file transfer related
    QString mFileUuid;
    int mTotalSeqCount;
    QFile* mFile;
    int mCurrentSeqNo;
};
