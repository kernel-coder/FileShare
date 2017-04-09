TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp \
    StatusViewer.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
include ($$PWD/Network/Network.pri)
include ($$PWD/QtJson/QtJson.pri)

HEADERS += \
    StatusViewer.h
