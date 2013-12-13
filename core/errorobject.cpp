#include "errorobject.h"

using namespace Ilwis;

ErrorObject::ErrorObject(const QString &message): _message(message)
{
}

ErrorObject::~ErrorObject() throw()
{
}

QString ErrorObject::message() const
{
    return _message;
}

const char *ErrorObject::what() const throw()
{
    return "ErrorObject";
}

InternalError::InternalError(const QString &message) : ErrorObject(message)
{
}

const char *InternalError::what() const throw()
{
    return "InternalError";
}

ScriptSyntaxError::ScriptSyntaxError(const QString &message) : ScriptError(message)
{
}

const char *ScriptSyntaxError::what() const throw()
{
    return "ScriptSyntaxError";
}

ScriptExecutionError::ScriptExecutionError(const QString &message) : ScriptError(message)
{
}

const char *ScriptExecutionError::what() const throw()
{
    return "ScriptExecutionError";
}

ScriptError::ScriptError(const QString &message) : ErrorObject(message)
{
}

const char *ScriptError::what() const throw()
{
    return "ScriptError";
}

FeatureCreationError::FeatureCreationError(const QString &message) : ErrorObject(message)
{
}

const char *FeatureCreationError::what() const throw()
{
    return "FeatureCreationError";
}

GeometryError::GeometryError(const QString &message) : ErrorObject(message)
{
}

const char *GeometryError::what() const throw()
{
    return "GeometryError";
}
