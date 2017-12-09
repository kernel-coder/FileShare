#include "FileTransferManager.h"
#include <QCoreApplication>
#include "NetworkManager.h"
#include "Connection.h"
#include <QDebug>
#include "Messages/ShareRequestMsg.h"
#include "Messages/ShareResponseMsg.h"
#include "Messages/FileTransferMsg.h"
#include "Messages/FilePartTransferMsg.h"
#include "Messages/ChatMsg.h"
#include "FileTransferUIInfoHandler.h"
#include "FileSenderHandler.h"
#include "FileReceiverHandler.h"
#include "Utils.h"


struct FileTransferManagerPri {
};


FileTransferManager* FileTransferManager::me()
{
    static FileTransferManager* _gftm = nullptr;
    if (_gftm == nullptr) {
        _gftm = new FileTransferManager(qApp);
    }
    return _gftm;
}


FileTransferManager::FileTransferManager(QObject* p)
    : JObject(p), d(new FileTransferManagerPri)
{
    connect(NetMgr, SIGNAL(newMsgCome(Connection*, Message*)), SLOT(onNewMsgCome(Connection*, Message*)));
}


void FileTransferManager::shareFilesTo(Connection *conn, const QList<QUrl> &urls)
{
    qDebug() << urls.first().toString();
    QStringList files = Utils::me()->urlsToFiles(urls);
    foreach (QString rootFile, files) {
        FileSenderHandler* handler = new FileSenderHandler(conn, QStringList(rootFile));
        FileMgrUIHandler->addSenderHandler(conn, handler);
        handler->start();
    }
}


void FileTransferManager::sendChatTo(Connection *conn, const QString &msg)
{
    ChatMsg* chatMsg = new ChatMsg(msg);
    conn->sendMessage(chatMsg);
    emit chatTransfer(conn, UITransferInfoItem::create(conn, msg, true));
}


void FileTransferManager::onNewMsgCome(Connection *sender, Message *msg)
{
    if (msg->typeId() == FileTransferHeaderInfoMsg::TypeID) {
        FileReceiverHandler* handler = new FileReceiverHandler(sender, qobject_cast<FileTransferHeaderInfoMsg*>(msg));
        FileMgrUIHandler->addReceiverHandler(sender, handler, qobject_cast<FileTransferHeaderInfoMsg*>(msg));
        handler->start();
    }

    if (msg->typeId() == ChatMsg::TypeID) {
        emit chatTransfer(sender, UITransferInfoItem::create(sender, (qobject_cast<ChatMsg*>(msg))->string(), false));
        msg->deleteLater();
    }
}
