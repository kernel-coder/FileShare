#include "StatusViewer.h"
#include <QCoreApplication>
#include <QDebug>

StatusViewer* StatusViewer::mpMe = NULL;

StatusViewer::StatusViewer(QObject *pParent):QObject(pParent)
{
    connect(&mTimer, &QTimer::timeout, [=] () {
        this->mTimer.stop();
        emit msgChanged("");
    });
}


StatusViewer* StatusViewer::me()
{
    if(mpMe == NULL){
        mpMe = new StatusViewer(qApp);
    }

    return mpMe;
}


void StatusViewer::showTip(QString msg, int timeout)
{
    qDebug() << "Showing tip " << msg << " for " << timeout << " ms";
    mCurrentMsg = msg;
    emit msgChanged(msg);
    mTimer.stop();
    if (timeout > 0) {
        mTimer.setInterval(timeout);
        mTimer.start();
    }
}
