#pragma once


#include <QMessageLogContext>
#include <QStringList>

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
void appCleanup();

