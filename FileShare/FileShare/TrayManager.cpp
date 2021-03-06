#include "TrayManager.h"
#include <QMenu>
#include <QApplication>
#include <QQmlEngine>

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
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    setIcon(QIcon(QPixmap(":/images/rsrc/images/btn-install.png")));

    QMenu* menu = new QMenu;
    menu->addAction("Quit", []() {
        qApp->quit();
    });
    setContextMenu(menu);

    connect(this, &QSystemTrayIcon::activated, [=](QSystemTrayIcon::ActivationReason reason) {
        if (reason == DoubleClick) {
            emit activateWindow();
        }
    });
    show();
    setToolTip("LAN Sharing");
}


void TrayManager::showAppMessage(const QString &title, const QString &msg, const QString &description)
{
    emit alertAppMessage(title, msg, description);
}
