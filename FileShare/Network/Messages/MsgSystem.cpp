#include "MsgSystem.h"
#include "Message.h"
#include "ServerInfoMsg.h"
#include "PeerViewInfoMsg.h"
#include "ShareRequestMsg.h"
#include "ShareResponseMsg.h"
#include "TransferControlMsg.h"
#include "ChatMsg.h"
#include "FileTransferMsg.h"
#include "FilePartTransferMsg.h"
#include <QDebug>


static int _msgIdCounter = 0;
int ServerInfoMsg::TypeID = _msgIdCounter++;
int PeerViewInfoMsg::TypeID = _msgIdCounter++;
int ShareRequestMsg::TypeID = _msgIdCounter++;
int ShareResponseMsg::TypeID = _msgIdCounter++;
int FileTransferHeaderInfoMsg::TypeID = _msgIdCounter++;
int FileTransferHeaderInfoAckMsg::TypeID = _msgIdCounter++;
int FileTransferMsg::TypeID = _msgIdCounter++;
int FileTransferAckMsg::TypeID = _msgIdCounter++;
int FilePartTransferMsg::TypeID = _msgIdCounter++;
int FilePartTransferAckMsg::TypeID = _msgIdCounter++;
int TransferControlMsg::TypeID = _msgIdCounter++;


MsgSystem::MsgSystem(QObject *parent) :
    QObject(parent)
{
}


Message * MsgSystem::readAndContruct(QDataStream &stream)
{
    Message *msg = NULL;
    int msgTypeId;

    stream >> msgTypeId;    

    if(msgTypeId == ServerInfoMsg::TypeID){
        msg = new ServerInfoMsg();
    }
    else if(msgTypeId == PeerViewInfoMsg::TypeID){
        msg = new PeerViewInfoMsg();
    }    
    else if(msgTypeId == ShareRequestMsg::TypeID){
        msg = new ShareRequestMsg();
    }
    else if(msgTypeId == ShareResponseMsg::TypeID){
        msg = new ShareResponseMsg();
    }
    else if (msgTypeId == FileTransferHeaderInfoMsg::TypeID){
        msg = new FileTransferHeaderInfoMsg();
    }
    else if (msgTypeId == FileTransferHeaderInfoAckMsg::TypeID){
        msg = new FileTransferHeaderInfoAckMsg();
    }
    else if(msgTypeId == FileTransferMsg::TypeID){
        msg = new FileTransferMsg();
    }
    else if(msgTypeId == FileTransferAckMsg::TypeID){
        msg = new FileTransferAckMsg();
    }
    else if(msgTypeId == FilePartTransferMsg::TypeID){
        msg = new FilePartTransferMsg();
    }
    else if(msgTypeId == FilePartTransferAckMsg::TypeID){
        msg = new FilePartTransferAckMsg();
    }
    else if (msgTypeId == ChatMsg::TypeID) {
        msg = new ChatMsg();
    }
    else if (msgTypeId == TransferControlMsg::TypeID) {
        msg = new TransferControlMsg();
    }

    if(msg){
        msg->read(stream);
    }

    return msg;
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
