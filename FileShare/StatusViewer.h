#ifndef STATUSVIEWER_H
#define STATUSVIEWER_H

#include <QObject>
#include <QTimer>

#define LONG_DURATION 5000
#define SHORT_DURATION 2000

class StatusViewer : public QObject
{
    Q_OBJECT
    StatusViewer(QObject *pParent = 0);
public:
    void showTip(QString msg, int timeout = 5000);
    static StatusViewer *me();

signals:
    void msgChanged(const QString& msg);

private:
    QString mCurrentMsg;
    QTimer mTimer;
    static StatusViewer *mpMe;

};

#endif // STATUSVIEWER_H
