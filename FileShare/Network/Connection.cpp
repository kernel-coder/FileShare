#include "Connection.h"
#include "Messages/Message.h"
#include "Messages/PeerViewInfoMsg.h"
#include "Messages/MsgSystem.h"
#include <QMutexLocker>
#include <QSysInfo>

Connection::Connection(int nSocketDesciptor,ServerInfoMsg::MyStatus status,QObject *parent) :
    QTcpSocket(parent),mnBlockSize(0),mpPeerViewInfo(NULL),mStatus(status)
{
    QObject::connect(this, SIGNAL(readyRead()), this, SLOT(dataReadyToRead()));

    if(setSocketDescriptor(nSocketDesciptor)){
        sendClientViewInfo();
    }
}

Connection::Connection(ServerInfoMsg::MyStatus status,QObject *parent):
        QTcpSocket(parent),mnBlockSize(0),mpPeerViewInfo(NULL),mStatus(status)
{
    QObject::connect(this, SIGNAL(readyRead()), this, SLOT(dataReadyToRead()));
    QObject::connect(this, SIGNAL(connected()),this,SLOT(sendClientViewInfo()));
}

ServerInfoMsg::MyStatus Connection::status()
{
    return mStatus;
}

void Connection::setStatus(ServerInfoMsg::MyStatus status)
{
    if(mStatus != status){
        mStatus = status;
        emit statusChanged(this);
    }
}



void Connection::sendClientViewInfo()
{
    PeerViewInfoMsg pvi(QSysInfo::machineHostName());
    sendMessage(&pvi);
}

bool Connection::sendMessage(Message *pMsg)
{
    if(pMsg){
        QByteArray block;
        QDataStream stream(&block, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_4_6);
        stream << (quint16)0;
        pMsg->write(stream);
        stream.device()->seek(0);
        stream << (quint16)(block.size() - sizeof(quint16));

        return this->write(block) == block.size();
    }

    return false;
}

void Connection::dataReadyToRead()
{
    QMutexLocker locker(&mMutex);

    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_6);

    while(bytesAvailable() >= (int)sizeof(quint16)){

        if (mnBlockSize == 0){
            in >> mnBlockSize;
        }

        if (bytesAvailable() < mnBlockSize){
            return;
        }

        Message *pMsg = MsgSystem::readAndContruct(in);

        if(pMsg){            
            if(pMsg->typeId() == PeerViewInfoMsg::TypeID){
                setPeerViewInfo(dynamic_cast<PeerViewInfoMsg*>(pMsg));
            }
            else{
                emit newMessageArrived(this,pMsg);
            }

            mnBlockSize = 0;
        }
    }
}

void Connection::setPeerViewInfo(PeerViewInfoMsg *pPeerViewInfo)
{
    if(mpPeerViewInfo != pPeerViewInfo && pPeerViewInfo != NULL){
        if(mpPeerViewInfo){
            delete mpPeerViewInfo;
            mpPeerViewInfo = NULL;
        }
        else{
            mpPeerViewInfo = pPeerViewInfo;
            emit readyForUse();
        }

        mpPeerViewInfo = pPeerViewInfo;
        emit peerViewInfoChanged();
    }
}

PeerViewInfoMsg *Connection::peerViewInfo()const{return mpPeerViewInfo;}

