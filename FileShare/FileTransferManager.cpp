#include "FileTransferManager.h"
#include <QCoreApplication>
#include "NetworkManager.h"
#include "Connection.h"
#include <QDebug>
#include "Messages/ShareRequestMsg.h"
#include "Messages/ShareResponseMsg.h"
#include "Messages/FileTransferMsg.h"
#include "Utils.h"
#include <QHash>
#include <QPair>

static int gRequestIdCounter = 1;
typedef QPair<Connection*, QStringList> RequestInfo;

struct FileTransferManagerPri {
    QHash<int, RequestInfo> RequestQueue;
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
    connect(NetMgr, SIGNAL(newMsgCame(Connection*, Message*)), SLOT(onNewMsgCame(Connection*,Message*)));
}


void FileTransferManager::shareFilesTo(Connection *conn, const QList<QUrl> &urls)
{
    qDebug() << urls.first().toString();
    QStringList files = Utils::me()->urlsToFiles(urls);
    foreach (QString file, files) {
        FileTransferMsg ftm(file);
        NetMgr->sendMessage(conn, &ftm);
    }
//    int requestId = gRequestIdCounter++;
//    d->RequestQueue[requestId] = RequestInfo(conn, files);
//    ShareRequestMsg msg(requestId, files);
//    NetMgr->sendMessage(conn, &msg);
}


void FileTransferManager::onNewMsgCame(Connection *sender, Message *msg)
{
//    switch(msg->typeId()) {
//        case ShareRequestMsg::TypeID:
//            break;
//        case ShareResponseMsg::TypeID:
//            break;
//        case FileTransferMsg::TypeID:
//        break;
//    }
}
