#include <QGuiApplication>
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
#include "FileTransferManager.h"
#include "FileTransferHandlers.h"
#include "AppSettings.h"


void registersQmlComponents()
{
    qmlRegisterType<Connection>("com.kcl.fileshare", 1, 0, "Connection");
    qmlRegisterType<PeerViewInfoMsg>("com.kcl.fileshare", 1, 0, "PeerViewInfoMsg");
    qmlRegisterType<RootFileUIInfo>("com.kcl.fileshare", 1, 0, "RootFileUIInfo");
}


static QObject* utilsProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return Utils::me();
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

void registersSingletonObjects()
{
    qmlRegisterSingletonType<AppSettings>("com.kcl.fileshare", 1, 0, "AppSettings", appSettingsProvider);
    qmlRegisterSingletonType<Utils>("com.kcl.fileshare", 1, 0, "Utils", utilsProvider);
    qmlRegisterSingletonType<NetworkManager>("com.kcl.fileshare", 1, 0, "NetMgr", netMgrProvider);
    qmlRegisterSingletonType<FileTransferManager>("com.kcl.fileshare", 1, 0, "FileMgr", fileMgrProvider);
    qmlRegisterSingletonType<FileTransferUIInfoHandler>("com.kcl.fileshare", 1, 0, "FileMgrUIHandler", fileMgrUIHandlerProvider);
}


int main(int argc, char *argv[])
{
    atexit(appCleanup);
    qRegisterMetaType<QMap<QString, QByteArray>>("QMap<QString,QByteArray>");
    qRegisterMetaType<QHostAddress>("QHostAddress");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    qInstallMessageHandler(customMessageHandler);

    QGuiApplication app(argc, argv);
    app.setOrganizationName("KCL");
    app.setApplicationName("QFileShare");

    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);
    QsLogging::DestinationPtr debugDestination(
           QsLogging::DestinationFactory::MakeDebugOutputDestination() );
        logger.addDestination(debugDestination.get());

    const QString sLogPath = app.applicationDirPath() + "/log-fs.txt";
    QsLogging::DestinationPtr fileDestination(
         QsLogging::DestinationFactory::MakeFileDestination(sLogPath));
    logger.addDestination(fileDestination.get());

    registersQmlComponents();
    registersSingletonObjects();

    AppSettings::me();
    NetworkManager::me();
    FileTransferManager::me();
    FileTransferUIInfoHandler::me();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/rsrc/qml/main.qml")));    

    return app.exec();
}
