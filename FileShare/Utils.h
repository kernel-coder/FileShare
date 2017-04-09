#pragma once

#include <QObject>
#include <QUrl>

class Utils : public QObject
{
    Q_OBJECT
public:
    explicit Utils(QObject *parent = 0);

    Q_INVOKABLE QString fileRelativeAppPath(const QString& file);
    Q_INVOKABLE QUrl urlRelativeAppPath(const QString& file);

};

