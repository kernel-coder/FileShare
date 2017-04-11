#include "Utils.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>


Utils* Utils::me()
{
    static Utils* _gutils = 0;
    if (_gutils == 0) {
        _gutils = new Utils(qApp);
    }
    return _gutils;
}

Utils::Utils(QObject *parent) : QObject(parent)
{
}


bool Utils::makePath(const QString& path)
{
    bool result = true;
    QDir dir(path);

    if (!dir.exists()) {
        result = dir.mkpath(path);
        if(!result) {
            qWarning() << "Failed to create path: " << path;
        }
    }

    return result;
}


QString Utils::dataDirCommon(const QString& file)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    path = QString("%1/%2/%3/").arg(path).arg(qApp->organizationName()).arg(qApp->applicationName());
    makePath(path);
    if (!file.isEmpty()) {
        path  = path + file;
    }
    return path;
}


QString Utils::fileRelativeAppPath(const QString &file)
{
    return QString("file:///%1/%2").arg(qApp->applicationDirPath().replace('\\', '/')).arg(file);
}


QUrl Utils::urlRelativeAppPath(const QString &file)
{
    return QUrl::fromLocalFile(QString("%1/%2").arg(qApp->applicationDirPath().replace('\\', '/')).arg(file));
}


QString Utils::urlToFile(const QUrl &url)
{
    if (url.isLocalFile()) {
        return url.toLocalFile();
    }
    return "invalid local url";
}


QStringList Utils::urlsToFiles(const QList<QUrl> &urls)
{
    QStringList files;
    foreach (QUrl url, urls) {
        files << urlToFile(url);
    }
    return files;
}
