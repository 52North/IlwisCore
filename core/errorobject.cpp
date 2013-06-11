#include <QString>
#include "errorobject.h"

using namespace Ilwis;

ErrorObject::ErrorObject(const QString& m) :
    QException(), _message(m)
{
}

ErrorObject::~ErrorObject() throw()
{
}

QString ErrorObject::message() const
{
    return _message;
}

InternalError::InternalError(const QString &message) : ErrorObject(message)
{
}


ScriptSyntaxError::ScriptSyntaxError(const QString &message) : ErrorObject(message)
{
}
