#include "Message.h"
#include <QtQml>


Message::Message(QObject* p) : JObject( p)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


Message::~Message()
{

}
