#ifndef ERROROBJECT_H
#define ERROROBJECT_H

#include "kernel_global.h"

#include <exception>
#include <QString>

namespace Ilwis {

/*!
 * \brief The ErrorObject class base class for all exceptions in Ilwis
 *
 *Exceptions are not widely used in ILwis, there is more a reliance on return values. But were they are used they must be derived from ErrorObject
 *The reason is that there must still be logging to the issuelogger and this can be done through the errorobject
 */
class KERNELSHARED_EXPORT ErrorObject :public std::exception{
public:
    /*!
     * \brief ErrorObject constructor
     * \param message A string message describing the error that occured
     */
    ErrorObject(const QString& message, bool log = true);
    ~ErrorObject() throw();
    /*!
     * \brief message
     * \return returns the message of the exception
     */
    QString message() const;
    virtual const char* what() const throw();

private:
    QString _message;

};

class KERNELSHARED_EXPORT InternalError : public ErrorObject {
public:
    explicit InternalError(const QString& message);
    const char* what() const throw();
};

class KERNELSHARED_EXPORT ScriptError : public ErrorObject {
public:
    explicit ScriptError(const QString& message);
    virtual const char* what() const throw();
};
class KERNELSHARED_EXPORT ScriptSyntaxError : public ScriptError {
public:
    explicit ScriptSyntaxError(const QString& message);
    const char* what() const throw();
};

class KERNELSHARED_EXPORT ScriptExecutionError : public ScriptError {
public:
    explicit ScriptExecutionError(const QString& message);
    const char* what() const throw();
};

class KERNELSHARED_EXPORT FeatureCreationError : public ErrorObject {
public:
    explicit FeatureCreationError(const QString& message);
    const char* what() const throw();
};

class KERNELSHARED_EXPORT GeometryError : public ErrorObject {
public:
    explicit GeometryError(const QString& message);
    const char* what() const throw();
};

class KERNELSHARED_EXPORT OutOfMemoryError : public ErrorObject {
public:
    explicit OutOfMemoryError(const QString& message, bool log = true);
    const char* what() const throw();
};

class KERNELSHARED_EXPORT CheckExpressionError : public ErrorObject {
public:
    explicit CheckExpressionError(const QString& message);
    const char* what() const throw();
};

class KERNELSHARED_EXPORT AbortOperation : public ErrorObject {
public:
    explicit AbortOperation(const QString& message);
    virtual const char* what() const throw();
};

class KERNELSHARED_EXPORT VisualizationError : public ErrorObject {
public:
    explicit VisualizationError(const QString& message);
    virtual const char* what() const throw();
};
}

#endif // ERROROBJECT_H
