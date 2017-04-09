#include "Utils.h"
#include <QCoreApplication>
#include <QDebug>

Utils::Utils(QObject *parent) : QObject(parent)
{
}


QString Utils::fileRelativeAppPath(const QString &file)
{
    return QString("file:///%1/%2").arg(qApp->applicationDirPath().replace('\\', '/')).arg(file);
}


QUrl Utils::urlRelativeAppPath(const QString &file)
{
    return QUrl::fromLocalFile(QString("%1/%2").arg(qApp->applicationDirPath().replace('\\', '/')).arg(file));
}
