#include "kernel.h"
#include "errorobject.h"


using namespace Ilwis;

ErrorObject::ErrorObject(const QString &message, bool log): _message(message)
{
    if (log)
        kernel()->issues()->log(message);
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

OutOfMemoryError::OutOfMemoryError(const QString &message, bool log) : ErrorObject(message, log)
{
}

const char *OutOfMemoryError::what() const throw()
{
    return "OutOfMemoryError";
}

CheckExpressionError::CheckExpressionError(const QString &message) : ErrorObject(message)
{
}

const char *CheckExpressionError::what() const throw()
{
    return "CheckExpressionError";
}


AbortOperation::AbortOperation(const QString &message) : ErrorObject(message)
{

}

const char *AbortOperation::what() const throw()
{
    return "AbortOperation";
}


VisualizationError::VisualizationError(const QString &message) : ErrorObject(message)
{
}

const char *VisualizationError::what() const throw()
{
    return "VisualizationError";
}
