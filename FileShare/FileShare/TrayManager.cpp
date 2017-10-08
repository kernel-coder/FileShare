#include "TrayManager.h"
#include <QMenu>
#include <QApplication>

class TrayManagerPrivate {
public:
    static TrayManager* Parent;

};


TrayManager* TrayManagerPrivate::Parent = nullptr;


TrayManager* TrayManager::me()
{
    if (TrayManagerPrivate::Parent == nullptr) {
        TrayManagerPrivate::Parent = new TrayManager(qApp);
    }
    return TrayManagerPrivate::Parent;
}


TrayManager::TrayManager(QObject *parent) : QSystemTrayIcon(parent)
{
#ifdef _WIN32
    setIcon(QIcon("qrc:/images/rsrc/images/lan16.icon"));
#else
    setIcon(QIcon(QPixmap("qrc:/images/rsrc/images/lan24.png")));
#endif

    QMenu* menu = new QMenu;
    menu->addAction("Quite", []() {
        qApp->quit();
    });
    setContextMenu(menu);

    connect(this, &QSystemTrayIcon::activated, [=](QSystemTrayIcon::ActivationReason reason) {
        if (reason == DoubleClick) {
            emit activeWindow();
        }
    });
    show();
    setToolTip("LAN Sharing");
}
