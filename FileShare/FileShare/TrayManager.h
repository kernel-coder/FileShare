#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H

#include <QSystemTrayIcon>

#define TrayMgr TrayManager::me()

class TrayManager : public QSystemTrayIcon
{
    Q_OBJECT
    explicit TrayManager(QObject *parent = 0);
public:
    static TrayManager* me();

signals:
    void activateWindow();

public slots:
};

#endif // TRAYMANAGER_H
