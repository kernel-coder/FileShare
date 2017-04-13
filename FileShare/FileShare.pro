TEMPLATE = app

QT += qml quick
CONFIG += c++11
# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =


# Default rules for deployment.
include(deployment.pri)
include ($$PWD/QsLog/QsLog.pri)
include ($$PWD/QtJson/QtJson.pri)
include ($$PWD/Network/Network.pri)
include ($$PWD/FileShare/FileShare.pri)

RESOURCES += qml.qrc

