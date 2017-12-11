#pragma once
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
    void alertAppMessage(const QString& title, const QString& msg, const QString& description);

public slots:
    void showAppMessage(const QString& title, const QString& msg, const QString& description = "");
};
