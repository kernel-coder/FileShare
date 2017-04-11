#include "Connection.h"
#include "Messages/Message.h"
#include "Messages/PeerViewInfoMsg.h"
#include "Messages/MsgSystem.h"
#include "NetworkManager.h"
#include <QMutexLocker>
#include <QSysInfo>
#include <QtQml>
#include <QTimer>


Connection::Connection(int sockId, QObject *parent)
    : QTcpSocket(parent)
    , mBlockSize(0)
    , _peerViewInfo(0)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    connect(this, SIGNAL(readyRead()), SLOT(dataReadyToRead()));

    if (sockId > 0 ) {
        if(setSocketDescriptor(sockId)) {
            sendClientViewInfo();
        }
    }
    else {
        connect(this, SIGNAL(connected()), SLOT(sendClientViewInfo()));
    }
}


void Connection::sendClientViewInfo()
{
    PeerViewInfoMsg* pvi = new PeerViewInfoMsg(NetMgr->username(), NetMgr->port(), NetMgr->status());
    sendMessage(pvi);
}


bool Connection::sendMessage(Message *msg)
{
    bool result = false;
    if(msg){
        QByteArray block;
        QDataStream stream(&block, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_4_6);
        stream << (quint64)0;
        msg->write(stream);
        stream.device()->seek(0);
        stream << (quint64)(block.size() - sizeof(quint64));
        result = this->write(block) == block.size();
        msg->deleteLater();
    }
    return result;
}


void Connection::dataReadyToRead()
{
    QMutexLocker locker(&mMutex);
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_6);

    while(bytesAvailable() >= (int)sizeof(quint64)) {

        if (mBlockSize == 0) {
            in >> mBlockSize;
        }

        if (bytesAvailable() < mBlockSize){
            return;
        }

        Message *msg = MsgSystem::readAndContruct(in);

        if(msg){
            if(msg->typeId() == PeerViewInfoMsg::TypeID){
                PeerViewInfoMsg* pvi = qobject_cast<PeerViewInfoMsg*>(msg);
                if(pvi != NULL){
                    if(_peerViewInfo){
                        _peerViewInfo->name(pvi->name());
                        _peerViewInfo->status(pvi->status());
                        pvi->deleteLater();
                    }
                    else {
                        peerViewInfo(pvi);
                        emit readyForUse();
                        qDebug() << " readyForUse fired";
                    }
                }
            }
            else{
                emit newMessageArrived(this, msg);
            }

            mBlockSize = 0;
        }
    }
}


