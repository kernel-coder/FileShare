#pragma once

#include <QObject>
#include <QUrl>

class Utils : public QObject
{
    Q_OBJECT
    explicit Utils(QObject *parent = 0);
public:
    static Utils* me();
    Q_INVOKABLE bool makePath(const QString& path);
    Q_INVOKABLE QString dataDirCommon(const QString& file = QString(""));
    Q_INVOKABLE QString fileRelativeAppPath(const QString& file);
    Q_INVOKABLE QUrl urlRelativeAppPath(const QString& file);
    Q_INVOKABLE QString urlToFile(const QUrl& url);
    Q_INVOKABLE QStringList urlsToFiles(const QList<QUrl>& url);
    Q_INVOKABLE QString formatSize(qint64 size);
    Q_INVOKABLE QString formatTime(qint64 ms);
    Q_INVOKABLE void openUrl(const QString& path);

};

