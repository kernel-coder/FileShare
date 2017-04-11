#include "MsgSystem.h"
#include "Message.h"
#include "ServerInfoMsg.h"
#include "PeerViewInfoMsg.h"
#include "ShareRequestMsg.h"
#include "ShareResponseMsg.h"

#include "ChatMsg.h"
#include "PlayRequestMsg.h"
#include "PlayRequestResultMsg.h"
#include "GameCanceledMsg.h"
#include "LineAddedMsg.h"
#include "FileTransferMsg.h"
#include "FilePartTransferMsg.h"


static int _msgIdCounter = 0;
int ServerInfoMsg::TypeID = _msgIdCounter++;
int PeerViewInfoMsg::TypeID = _msgIdCounter++;
int ShareRequestMsg::TypeID = _msgIdCounter++;
int ShareResponseMsg::TypeID = _msgIdCounter++;
int FileTransferMsg::TypeID = _msgIdCounter++;
int FileTransferAckMsg::TypeID = _msgIdCounter++;
int FilePartTransferMsg::TypeID = _msgIdCounter++;
int FilePartTransferAckMsg::TypeID = _msgIdCounter++;

MsgSystem::MsgSystem(QObject *parent) :
    QObject(parent)
{
}


Message * MsgSystem::readAndContruct(QDataStream &stream)
{
    Message *pMsg = NULL;
    int msgTypeId;

    stream >> msgTypeId;

    if(msgTypeId == ServerInfoMsg::TypeID){
        pMsg = new ServerInfoMsg();
    }
    else if(msgTypeId == PeerViewInfoMsg::TypeID){
        pMsg = new PeerViewInfoMsg();
    }    
    else if(msgTypeId == ShareRequestMsg::TypeID){
        pMsg = new ShareRequestMsg();
    }
    else if(msgTypeId == ShareResponseMsg::TypeID){
        pMsg = new ShareResponseMsg();
    }
    else if(msgTypeId == FileTransferMsg::TypeID){
        pMsg = new FileTransferMsg();
    }
    else if(msgTypeId == FileTransferAckMsg::TypeID){
        pMsg = new FileTransferAckMsg();
    }
    else if(msgTypeId == FilePartTransferMsg::TypeID){
        pMsg = new FilePartTransferMsg();
    }
    else if(msgTypeId == FilePartTransferAckMsg::TypeID){
        pMsg = new FilePartTransferAckMsg();
    }
    else if(msgTypeId == ChatMsg::TypeID){
        pMsg = new ChatMsg();
    }
    else if(msgTypeId == GameCanceledMsg::TypeID){
        pMsg = new GameCanceledMsg();
    }
    else if(msgTypeId == LineAddedMsg::TypeID){
        pMsg = new LineAddedMsg();
    }

    if(pMsg){
        pMsg->read(stream);
    }

    return pMsg;
}


QString MsgSystem::readableStatus(PeerViewInfoMsg::PeerStatus status)
{
    QString strReadable;

    if(PeerViewInfoMsg::Free == status){
        strReadable = tr("free");
    }
    else if(PeerViewInfoMsg::Busy == status){
        strReadable = tr("busy");
    }

    return strReadable;
}
