#include "AppSettings.h"
#include <QSettings>
#include <QCoreApplication>


#define W_CHECK(x) qMax(680., x)
#define H_CHECK(x) qMax(480., x)

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
    _appWidth = W_CHECK(s.value("appWidth", 680).toReal());
    _appHeight = H_CHECK(s.value("appHeight", 480).toReal());
    _skippExistingFile = s.value("skippExistingFile", true).toBool();
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

    w = W_CHECK(w);

    if (w != _appWidth) {
        appWidth(w);
        s.setValue("appWidth", w);
    }

    h = H_CHECK(h);

    if (h != _appHeight) {
        appHeight(h);
        s.setValue("appHeight", h);
    }
}
