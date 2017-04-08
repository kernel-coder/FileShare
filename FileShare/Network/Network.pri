INCLUDEPATH += $$PWD $$PWD/Messages
QT += network

HEADERS += \
    #$$PWD/ChatWnd.h \
    $$PWD/Connection.h \
    $$PWD/NetworkManager.h \
    #$$PWD/PeerChatView.h \
    $$PWD/PeerManager.h \
    $$PWD/Server.h \
    $$PWD/Messages/ChatMsg.h \
    $$PWD/Messages/GameCanceledMsg.h \
    $$PWD/Messages/LineAddedMsg.h \
    $$PWD/Messages/Message.h \
    $$PWD/Messages/MsgSystem.h \
    $$PWD/Messages/PeerViewInfoMsg.h \
    $$PWD/Messages/PlayRequestMsg.h \
    $$PWD/Messages/PlayRequestResultMsg.h \
    $$PWD/Messages/ServerInfoMsg.h \
    $$PWD/Messages/NewFileRequestMsg.h

SOURCES += \
    #$$PWD/ChatWnd.cpp \
    $$PWD/Connection.cpp \
    $$PWD/NetworkManager.cpp \
    #$$PWD/PeerChatView.cpp \
    $$PWD/PeerManager.cpp \
    $$PWD/Server.cpp \
    $$PWD/Messages/ChatMsg.cpp \
    $$PWD/Messages/GameCanceledMsg.cpp \
    $$PWD/Messages/LineAddedMsg.cpp \
    $$PWD/Messages/Message.cpp \
    $$PWD/Messages/MsgSystem.cpp \
    $$PWD/Messages/PeerViewInfoMsg.cpp \
    $$PWD/Messages/PlayRequestMsg.cpp \
    $$PWD/Messages/PlayRequestResultMsg.cpp \
    $$PWD/Messages/ServerInfoMsg.cpp \
    $$PWD/Messages/NewFileRequestMsg.cpp
