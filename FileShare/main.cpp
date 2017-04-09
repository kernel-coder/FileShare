#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "NetworkManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/rsrc/qml/main.qml")));

    NetworkManager nm;

    return app.exec();
}
