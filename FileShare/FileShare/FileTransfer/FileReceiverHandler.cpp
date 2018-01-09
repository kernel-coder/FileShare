#include "FileReceiverHandler.h"
#include "NetworkManager.h"
#include "Connection.h"
#include <QCoreApplication>
#include <QDebug>
#include "Messages/ShareRequestMsg.h"
#include "Messages/ShareResponseMsg.h"
#include "Messages/FileTransferMsg.h"
#include "Messages/FilePartTransferMsg.h"
#include "Utils.h"
#include <QHash>
#include <QPair>
#include <QFileInfo>
#include <QDir>
#include <QFileInfoList>
#include <QUuid>
#include <QtMath>
#include <QThread>
#include "FileTransferManager.h"
#include "AppSettings.h"


FileReceiverHandler::FileReceiverHandler(Connection* conn, FileTransferHeaderInfoMsg *msg, QObject *p)
    : FileHandlerBase(conn, msg->transferId(), p)
    , mHeaderInfoMsg(msg)
    , mFile(0)
    , mFileFlushMark(0)
    , mFileSeqCount(0)
{
}


FileReceiverHandler::~FileReceiverHandler()
{
    if (mHeaderInfoMsg) {
        delete mHeaderInfoMsg;
    }
}


void FileReceiverHandler::handleInitialize()
{
    transferStatus(TransferStatusFlag::Running);
    auto headerAck = new FileTransferHeaderInfoAckMsg(mHeaderInfoMsg->transferId(),
                                                      mHeaderInfoMsg->filePath(),
                                                      mHeaderInfoMsg->fileCount(),
                                                      mHeaderInfoMsg->totalSize());
    emit sendMsg(headerAck);
}


void FileReceiverHandler::handleMessageComingFrom(Connection *conn, Message *msg)
{
    if (mConnection == conn) {
        if (msg->typeId() == FileTransferMsg::TypeID) {
            auto fMsg = qobject_cast<FileTransferMsg*>(msg);
            if (fMsg->transferId() == mHeaderInfoMsg->transferId()) {
                mFileFlushMark = 0;
                QString filename = FileMgr->saveFolderPathForTransferID(conn, fMsg->transferId());
                filename = filename.replace("\\", "/");
                if (!filename.endsWith("/")) filename += "/";
                filename += fMsg->basePath();
                Utils::me()->makePath(filename);
                if (!filename.endsWith("/")) filename += "/";
                filename += fMsg->filename();

                bool skipping = false;
                if (AppSettings::me()->skippExistingFile()) {
                    QFileInfo fi(filename);
                    if (fi.exists() && fi.size() == fMsg->size()) {
                        skipping = true;
                        emit transferInfoUpdated(mConnection, fMsg->transferId(), fMsg->progressSize(), fMsg->fileNo());
                    }
                }

                if (!skipping) {
                    mFileUUID = fMsg->uuid();
                    mFileSeqCount = fMsg->seqCount();
                    mFile = new QFile(filename);
                    qDebug() << "Receving file: "  << filename;
                    if (fMsg->startPos() > 0) {
                        if (mFile->open(QFile::Append)) {
                            qint64 cp = mFile->pos();
                            mFile->seek(0);
                            cp = mFile->pos();
                            mFile->seek(fMsg->startPos());
                            cp = mFile->pos();
                        }
                    }
                    else {
                        mFile->open(QFile::WriteOnly);
                    }
                }

                FileTransferAckMsg* ackMsg = new FileTransferAckMsg(fMsg->transferId(),
                                                                    fMsg->uuid(),
                                                                    fMsg->filename(),
                                                                    skipping,
                                                                    fMsg->startPos());
                ackMsg->basePath(fMsg->basePath());
                ackMsg->size(fMsg->size());
                ackMsg->progressSize(fMsg->progressSize());
                ackMsg->seqCount(fMsg->seqCount());
                ackMsg->fileNo(fMsg->fileNo());
                emit sendMsg(ackMsg);
                fMsg->deleteLater();
            }
        }
        else if (msg->typeId() == FilePartTransferMsg::TypeID) {
           FilePartTransferMsg* fptm = qobject_cast<FilePartTransferMsg*>(msg);
           if (fptm->uuid() == mFileUUID) {
               mFileFlushMark++;
               mFile->write(fptm->data());
               if (mFileFlushMark * MSG_LEN / ONE_MB >= 100) {
                   // if it is 100 MB, lets flush it
                   mFile->flush();
                   mFileFlushMark = 0;
               }
               FilePartTransferAckMsg* ackMsg = new FilePartTransferAckMsg(fptm->transferId(), fptm->uuid(), fptm->fileNo(), fptm->seqNo(), fptm->size(), fptm->progressSize());
               emit transferInfoUpdated(mConnection, ackMsg->transferId(), ackMsg->progressSize(), ackMsg->fileNo());
               emit sendMsg(ackMsg);

               if (fptm->seqNo() + 1 == mFileSeqCount) {
                   qDebug() << "File Received: "  << mFile->fileName();
                   mFile->flush();
                   mFile->close();
                   mFile->deleteLater();
                   if (fptm->fileNo() == mHeaderInfoMsg->fileCount()) {
                       destroyMyself(TransferStatusFlag::Finished);
                   }
               }

               msg->deleteLater();
           }           
        }
    }
}
