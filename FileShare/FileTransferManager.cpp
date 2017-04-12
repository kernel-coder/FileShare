#include "FileTransferManager.h"
#include <QCoreApplication>
#include "NetworkManager.h"
#include "Connection.h"
#include <QDebug>
#include "Messages/ShareRequestMsg.h"
#include "Messages/ShareResponseMsg.h"
#include "Messages/FileTransferMsg.h"
#include "Messages/FilePartTransferMsg.h"
#include "FileTransferHandlers.h"
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
    FileSenderHandler* handler = new FileSenderHandler(conn, files);
    handler->start();
}


void FileTransferManager::onNewMsgCome(Connection *sender, Message *msg)
{
    if (msg->typeId() == FileTransferMsg::TypeID) {
        FileReceiverHandler* handler = new FileReceiverHandler(sender, qobject_cast<FileTransferMsg*>(msg));
        handler->start();
    }
}
