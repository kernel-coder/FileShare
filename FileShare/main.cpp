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

void registersQmlComponents()
{
    qmlRegisterType<Connection>("com.kcl.fileshare", 1, 0, "Connection");
    qmlRegisterType<PeerViewInfoMsg>("com.kcl.fileshare", 1, 0, "PeerViewInfoMsg");
}


static QObject* netMgrProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return NetworkManager::me();
}

static QObject* utilsProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new Utils(qApp);
}


void registersSingletonObjects()
{
    qmlRegisterSingletonType<NetworkManager>("com.kcl.fileshare", 1, 0, "Backend", netMgrProvider);
    qmlRegisterSingletonType<Utils>("com.kcl.fileshare", 1, 0, "Utils", utilsProvider);
}



int main(int argc, char *argv[])
{
    atexit(appCleanup);
    qRegisterMetaType<QMap<QString,QByteArray>>("QMap<QString,QByteArray>");
    qInstallMessageHandler(customMessageHandler);

    QGuiApplication app(argc, argv);

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

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/rsrc/qml/main.qml")));

    NetworkManager::me();

    return app.exec();
}
