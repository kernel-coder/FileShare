INCLUDEPATH += $$PWD

SOURCES += $$PWD/main.cpp \
    $$PWD/StatusViewer.cpp \
    $$PWD/Utils.cpp \    
    $$PWD/AppSettings.cpp \
    $$PWD/TrayManager.cpp \
    $$PWD/HistoryManager.cpp \    
    $$PWD/FileTransfer/FileHandlerBase.cpp \
    $$PWD/FileTransfer/FileSenderHandler.cpp \
    $$PWD/FileTransfer/FileReceiverHandler.cpp \
    $$PWD/FileTransfer/FileTransferUIInfoHandler.cpp \
    $$PWD/FileTransfer/FileTransferManager.cpp

HEADERS += \
    $$PWD/StatusViewer.h \
    $$PWD/Utils.h \
    $$PWD/AppSettings.h \
    $$PWD/TrayManager.h \
    $$PWD/HistoryManager.h \
    $$PWD/FileTransfer/FileHandlerBase.h \
    $$PWD/FileTransfer/FileSenderHandler.h \
    $$PWD/FileTransfer/FileReceiverHandler.h \
    $$PWD/FileTransfer/FileTransferUIInfoHandler.h \
    $$PWD/FileTransfer/FileTransferManager.h
