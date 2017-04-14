#include "AppSettings.h"
#include <QSettings>
#include <QCoreApplication>


AppSettings* AppSettings::me()
{
    static AppSettings* _gAppSettings = nullptr;
    if (_gAppSettings == nullptr) {
        _gAppSettings = new AppSettings(qApp);
    }
    return _gAppSettings;
}

AppSettings::AppSettings(QObject* p) : JObject(p)
{
    QSettings s;
    _appPosX = s.value("appPosX", 0).toReal();
    _appPosY = s.value("appPosY", 0).toReal();
    _appWidth = s.value("appWidth", 640).toReal();
    _appHeight = s.value("appHeight", 480).toReal();
}


AppSettings::~AppSettings()
{
   //updateAppGeometry(_appPosX, _appPosY, _appWidth, _appHeight);
}

void AppSettings::updateAppGeometry(qreal x, qreal y, qreal w, qreal h)
{
    QSettings s;
    if (x != _appPosX) {
        appPosX(x);
        s.setValue("appPosX", x);
    }

    if (y != _appPosY) {
        appPosY(y);
        s.setValue("appPosY", y);
    }

    if (w != _appWidth) {
        appWidth(w);
        s.setValue("appWidth", w);
    }

    if (h != _appHeight) {
        appHeight(h);
        s.setValue("appHeight", h);
    }
}
