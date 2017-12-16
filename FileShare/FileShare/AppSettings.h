#pragma once

#include "JObject.h"

class AppSettings : public JObject
{
    Q_OBJECT
    AppSettings(QObject* p = 0);
public:
    static AppSettings* me();
    ~AppSettings();
    MetaPropertyPublicSet_Ex(qreal, appPosX)
    MetaPropertyPublicSet_Ex(qreal, appPosY)
    MetaPropertyPublicSet_Ex(qreal, appWidth)
    MetaPropertyPublicSet_Ex(qreal, appHeight)
    MetaPropertyPublicSet_Ex(bool, skippExistingFile)

public slots:
    void updateAppGeometry(qreal x, qreal y, qreal w, qreal h);

};
