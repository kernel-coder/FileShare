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

RESOURCES += qml.qrc

SOURCES += main.cpp \
    StatusViewer.cpp \
    Utils.cpp \
    FileTransferManager.cpp \
    FileTransferHandlers.cpp

HEADERS += \
    StatusViewer.h \
    Utils.h \
    FileTransferManager.h \
    FileTransferHandlers.h
