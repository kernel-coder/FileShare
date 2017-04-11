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


static int _msgIdCounter = 0;
int ServerInfoMsg::TypeID = _msgIdCounter++;
int PeerViewInfoMsg::TypeID = _msgIdCounter++;
int ShareRequestMsg::TypeID = _msgIdCounter++;
int ShareResponseMsg::TypeID = _msgIdCounter++;
int FileTransferMsg::TypeID = _msgIdCounter++;


MsgSystem::MsgSystem(QObject *parent) :
    QObject(parent)
{
}


Message * MsgSystem::readAndContruct(QDataStream &stream)
{
    Message *pMsg = NULL;
    int nTypeId;

    stream >> nTypeId;

    if(nTypeId == ServerInfoMsg::TypeID){
        pMsg = new ServerInfoMsg();
    }
    else if(nTypeId == PeerViewInfoMsg::TypeID){
        pMsg = new PeerViewInfoMsg();
    }
    else if(nTypeId == ChatMsg::TypeID){
        pMsg = new ChatMsg();
    }
    else if(nTypeId == PlayRequestMsg::TypeID){
        pMsg = new PlayRequestMsg();
    }
    else if(nTypeId == PlayRequestResultMsg::TypeID){
        pMsg = new PlayRequestResultMsg();
    }
    else if(nTypeId == GameCanceledMsg::TypeID){
        pMsg = new GameCanceledMsg();
    }
    else if(nTypeId == LineAddedMsg::TypeID){
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
