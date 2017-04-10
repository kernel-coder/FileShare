#include "Connection.h"
#include "Messages/Message.h"
#include "Messages/PeerViewInfoMsg.h"
#include "Messages/MsgSystem.h"
#include "NetworkManager.h"
#include <QMutexLocker>
#include <QSysInfo>
#include <QtQml>
#include <QTimer>


Connection::Connection(int sockId, QObject *parent) :
    QTcpSocket(parent)
  , mBlockSize(0)
  , _peerViewInfo(0)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    connect(this, SIGNAL(readyRead()), SLOT(dataReadyToRead()));

    if (sockId > 0 ) {
        if(setSocketDescriptor(sockId)) {
            QTimer::singleShot(1000, [=](){
               sendClientViewInfo();
            });
        }
    }
    else {
        connect(this, &QTcpSocket::connected, [=]() {
            QTimer::singleShot(1000, [=](){
               sendClientViewInfo();
            });
        });
    }
}


void Connection::sendClientViewInfo()
{
    PeerViewInfoMsg pvi(NetworkManager::me()->username(), NetworkManager::me()->status());
    sendMessage(&pvi);
}


bool Connection::sendMessage(Message *pMsg)
{
    if(pMsg){
        QByteArray block;
        QDataStream stream(&block, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_4_6);
        stream << (quint64)0;
        pMsg->write(stream);
        stream.device()->seek(0);
        stream << (quint64)(block.size() - sizeof(quint64));

        return this->write(block) == block.size();
    }
    return false;
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

        Message *pMsg = MsgSystem::readAndContruct(in);

        if(pMsg){            
            if(pMsg->typeId() == PeerViewInfoMsg::TypeID){
                PeerViewInfoMsg* pvi = qobject_cast<PeerViewInfoMsg*>(pMsg);
                if(pvi != NULL){
                    if(_peerViewInfo){
                        _peerViewInfo->name(pvi->name());
                        _peerViewInfo->status(pvi->status());
                    }
                    else {
                        peerViewInfo(pvi);
                        emit readyForUse();
                    }
                }
            }
            else{
                emit newMessageArrived(this,pMsg);
            }

            mBlockSize = 0;
        }
    }
}


