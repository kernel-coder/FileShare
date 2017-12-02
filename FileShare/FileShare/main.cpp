#include <QApplication>
#include <QQmlApplicationEngine>
#include "NetworkManager.h"
#include "Connection.h"
#include "Network/Messages/PeerViewInfoMsg.h"
#include "Utils.h"
#include "QsLog.h"
#include "QsLogDest.h"
#include "CustomLogger.h"
#include <QDir>
#include <QtQml>
#include "FileTransfer/FileTransferManager.h"
#include "FileTransfer/FileTransferUIInfoHandler.h"
#include "AppSettings.h"
#include <QTime>
#include "TrayManager.h"
#include "HistoryManager.h"

void registersQmlComponents()
{
    qmlRegisterType<Connection>("com.kcl.fileshare", 1, 0, "Connection");
    qmlRegisterType<PeerViewInfoMsg>("com.kcl.fileshare", 1, 0, "PeerViewInfoMsg");
    qmlRegisterType<RootFileUIInfo>("com.kcl.fileshare", 1, 0, "RootFileUIInfo");
    qmlRegisterType<UITransferInfoItem>("com.kcl.fileshare", 1, 0, "UITransferInfoItem");
    qmlRegisterType<HistoryManager>("com.kcl.fileshare", 1, 0, "HistoryManager");
}


static QObject* utilsProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return Utils::me();
}


static QObject* trayMgrProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return TrayMgr;
}


static QObject* appSettingsProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return AppSettings::me();
}


static QObject* netMgrProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return NetMgr;
}


static QObject* fileMgrProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return FileMgr;
}


static QObject* fileMgrUIHandlerProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return FileMgrUIHandler;
}

static QObject* historyManagerProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return HistoryMgr;
}

void registersSingletonObjects()
{
    qmlRegisterSingletonType<AppSettings>("com.kcl.fileshare", 1, 0, "AppSettings", appSettingsProvider);
    qmlRegisterSingletonType<TrayManager>("com.kcl.fileshare", 1, 0, "TrayMgr", trayMgrProvider);
    qmlRegisterSingletonType<Utils>("com.kcl.fileshare", 1, 0, "Utils", utilsProvider);
    qmlRegisterSingletonType<NetworkManager>("com.kcl.fileshare", 1, 0, "NetMgr", netMgrProvider);
    qmlRegisterSingletonType<FileTransferManager>("com.kcl.fileshare", 1, 0, "FileMgr", fileMgrProvider);
    qmlRegisterSingletonType<FileTransferUIInfoHandler>("com.kcl.fileshare", 1, 0, "FileMgrUIHandler", fileMgrUIHandlerProvider);
    qmlRegisterSingletonType<HistoryManager>("com.kcl.fileshare", 1, 0, "HistoryMgr", historyManagerProvider);
}


int main(int argc, char *argv[])
{
    atexit(appCleanup);
    qRegisterMetaType<QMap<QString, QByteArray>>("QMap<QString,QByteArray>");
    qRegisterMetaType<QHostAddress>("QHostAddress");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    qRegisterMetaType<QList<UITransferInfoItem*>>("QList<UITransferInfoItem>");

    qInstallMessageHandler(customMessageHandler);

    QApplication app(argc, argv);
    app.setOrganizationName("KCL");
    app.setApplicationName("QFileShare");

    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);
    QsLogging::DestinationPtr debugDestination(
           QsLogging::DestinationFactory::MakeDebugOutputDestination() );
        logger.addDestination(debugDestination.get());

    const QString sLogPath = Utils::me()->findUniqueLogFilename("log-fs.txt");
    QsLogging::DestinationPtr fileDestination(
         QsLogging::DestinationFactory::MakeFileDestination(sLogPath));
    logger.addDestination(fileDestination.get());

    registersQmlComponents();
    registersSingletonObjects();

    TrayMgr->showMessage("LAN Sharing", "Starting...", QSystemTrayIcon::Information);

    AppSettings::me();
    NetworkManager::me();
    FileTransferManager::me();
    FileTransferUIInfoHandler::me();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/rsrc/qml/main.qml")));    



    return app.exec();
}
