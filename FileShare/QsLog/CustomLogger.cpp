#include "CustomLogger.h"
#include "QsLog.h"
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include <QDebug>

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    //QByteArray localMsg = msg.toLocal8Bit();
    QString fmsg = QString("%1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
    switch (type) {
    case QtDebugMsg:
        //fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        QLOG_DEBUG() << fmsg;
        break;
    case QtWarningMsg:
        //fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        QLOG_WARN() << fmsg;
        break;
    case QtCriticalMsg:
        //fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        QLOG_ERROR() << fmsg;
        break;
    case QtFatalMsg:
        //fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        QLOG_FATAL() << fmsg;
        abort();
    }
    fflush(stderr);
}


void appCleanup()
{
}
