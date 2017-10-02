#include "Utils.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QDateTime>

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


QString Utils::dataDirUser(const QString &file)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/";
    makePath(path);
    if (!file.isEmpty()) {
        path  = path + file;
    }
    return path;
}


QString Utils::logDirUser(const QString &fileName)
{
    QString path = dataDirUser() + "Log/";
    makePath(path);
    if (!fileName.isEmpty())
        path = path + fileName;
    return path;
}


QString Utils::findUniqueLogFilename(const QString& prefix)
{
    return findUniqueFilename(logDirUser(), prefix, "txt");
}


QString Utils::findUniqueFilename(const QString& dir, const QString& prefix, const QString& ext)
{
    int counter = 1;
    QString file = dir + QString("%1-%2.%3").arg(prefix).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss")).arg(ext);
    if (QFile::exists(file)) {
        file = dir + QString("%1-%2-%3.%4").arg(prefix).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss")).arg(counter).arg(ext);
        counter++;
    }
    return file;
}


QByteArray Utils::readFile(const QString &filename)
{
    QFile file(filename);
    if (file.open(QFile::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        return data;
    }
    return QByteArray();
}


bool Utils::writeFile(const QString &filename, const QByteArray &data)
{
    QFile file(filename);
    if (file.open(QFile::WriteOnly)) {
        file.write(data);
        return true;
    }
    return false;
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


QString Utils::formatSize(qint64 size)
{
    QString unit = "B";
    double s = size;
    if (s >= 1024) {
        s /= 1024;
        unit = "kB";
    }
    if (s >= 1024) {
        s /= 1024;
        unit = "MB";
    }
    if (s >= 1024) {
        s /= 1024;
        unit = "GB";
    }
    return QString("%1%2").arg(QString::number(s, 'f', 2)).arg(unit);
}


QString Utils::formatTime(qint64 ms)
{
    qint32 minute = ms / (1000 * 60);
    ms = ms % (1000 * 60);
    qint32 second = ms / 1000;
    //qint32 msec = ms % 1000;
    QString formated("%1:%2");
    return formated.arg(minute, 2, 10, QChar('0')).arg(second, 2, 10, QChar('0'));
}


void Utils::openUrl(const QString &path)
{
    QDesktopServices::openUrl(QUrl(path));
}
