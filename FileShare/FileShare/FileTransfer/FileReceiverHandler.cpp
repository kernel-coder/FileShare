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
#include "FileTransferUIInfoHandler.h"
#include "AppSettings.h"


FileReceiverHandler::FileReceiverHandler(Connection* conn, FileTransferHeaderInfoMsg *msg, QObject *p)
    : FileHandlerBase(conn, msg->transferId(), p)
    , mHeaderInfoMsg(msg)
    , mFileMsg(0)
    , mFile(0)
    , mFileFlushMark(0)
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
                mFileMsg = fMsg;
                QString filename = FileMgrUIHandler->saveFolderPathForTransferID(conn, mFileMsg->transferId());
                filename = filename.replace("\\", "/");
                if (!filename.endsWith("/")) filename += "/";
                filename += mFileMsg->basePath();
                Utils::me()->makePath(filename);
                if (!filename.endsWith("/")) filename += "/";
                filename += mFileMsg->filename();
                bool skipping = false;
                if (AppSettings::me()->skippExistingFile()) {
                    QFileInfo fi(filename);
                    if (fi.exists() && fi.size() == mFileMsg->size()) {
                        skipping = true;
                    }
                }

                FileTransferAckMsg* ackMsg = new FileTransferAckMsg(mFileMsg->transferId(),
                                                                    mFileMsg->uuid(),
                                                                    mFileMsg->filename(),
                                                                    skipping,
                                                                    mFileMsg->startPos());
                ackMsg->basePath(mFileMsg->basePath());
                ackMsg->size(mFileMsg->size());
                ackMsg->seqCount(mFileMsg->seqCount());
                ackMsg->fileNo(mFileMsg->fileNo());
                emit sendMsg(ackMsg);

                if (skipping) {
                    mFileMsg->deleteLater();
                    mFileMsg = 0;
                }
                else {
                    mFile = new QFile(filename);
                    qDebug() << "Receving file: "  << filename;
                    if (mFileMsg->startPos() > 0) {
                        if (mFile->open(QFile::Append)) {
                            qint64 cp = mFile->pos();
                            mFile->seek(0);
                            cp = mFile->pos();
                            mFile->seek(mFileMsg->startPos());
                            cp = mFile->pos();
                        }
                    }
                    else {
                        mFile->open(QFile::WriteOnly);
                    }
                }
            }
        }
        else if (msg->typeId() == FilePartTransferMsg::TypeID) {
           FilePartTransferMsg* fptm = qobject_cast<FilePartTransferMsg*>(msg);
           if (mFileMsg && fptm->uuid() == mFileMsg->uuid()) {
               mFileFlushMark++;
               mFile->write(fptm->data());
               if (mFileFlushMark * MSG_LEN / ONE_MB >= 100) {
                   // if it is 100 MB, lets flush it
                   mFile->flush();
                   mFileFlushMark = 0;
               }
               FilePartTransferAckMsg* ackMsg = new FilePartTransferAckMsg(fptm->transferId(), fptm->uuid(), fptm->fileNo(), fptm->seqNo(), fptm->size(), fptm->progressSize());
               emit receivedFilePart(mConnection, ackMsg);
               emit sendMsg(ackMsg);

               if (fptm->seqNo() + 1 == mFileMsg->seqCount()) {
                   qDebug() << "File Received: "  << mFile->fileName();
                   mFile->flush();
                   mFile->close();
                   mFile->deleteLater();
                   mFileMsg->deleteLater();
                   if (fptm->fileNo() == mHeaderInfoMsg->fileCount()) {
                       destroyMyself(TransferStatusFlag::Finished);
                   }
               }

               msg->deleteLater();
           }           
        }
    }
}
