#ifndef ERROROBJECT_H
#define ERROROBJECT_H

#include <QtCore>
#include "Kernel_global.h"

namespace Ilwis {

/*!
 * \brief The ErrorObject class base class for all exceptions in Ilwis
 *
 *Exceptions are not widely used in ILwis, there is more a reliance on return values. But were they are used they must be derived from ErrorObject
 *The reason is that there must still be logging to the issuelogger and this can be done through the errorobject
 */
class KERNELSHARED_EXPORT ErrorObject :public QException
{
public:
    /*!
     * \brief ErrorObject constructor
     * \param message A string message describing the error that occured
     */
    ErrorObject(const QString& message);
    ~ErrorObject() throw();
    /*!
     * \brief message
     * \return returns the message of the exception
     */
    QString message() const;

signals:

public slots:

private:
    QString _message;

};

class KERNELSHARED_EXPORT InternalError : public ErrorObject {
public:
    explicit InternalError(const QString& message);
};
}

#endif // ERROROBJECT_H
