#ifndef ILWISOBJECTFACTORY_H
#define ILWISOBJECTFACTORY_H

#include "kernel_global.h"
#include "ilwis.h"

namespace Ilwis {

class IlwisObject;
class Resource;
class IlwisObjectConnector;

/*!
 \brief creates ilwisobjects

 A general interface for factories for all kinds of ilwisobject. Note that is not needed that a factory can produce all types of objects.
 For example there exists a projectionfactory that only can produce projection objects. Factories are registered to the kernel and
 others can request a factory of a certain type from the kernel

*/
class KERNELSHARED_EXPORT IlwisObjectFactory : public AbstractFactory
{
public:
    /*!
 \brief constructor for a factory object

 The method 2 first parameters are used to choose which factory is used

 \param ty maintype of the factory. The type usualy signifies which kind objects it can produce though there is no special reason for it.
 \param sub subtype of the factory. Usualy used to make a choice between several types of factories that can produce the same kind of objects
 \param desc optional description of the factory.
*/
    IlwisObjectFactory(const QString& ty, const QString& sub, const QString& desc="");

    /*!
     \brief prototype for the mandatory creation method that all ilwisobject factories must suport

     \param resource a pointer the location of the resource from which the ilwisobject is created
     \return IlwisObject the created ilwisobject or 0 if something went wrong
    */

    virtual IlwisObject *create(const Resource &resource,const IOOptions& options=IOOptions()) const = 0;
    virtual IlwisObject *create(IlwisTypes, const QString& sub = sUNDEF ) const { return (sub == sUNDEF ? nullptr: nullptr); }
    virtual std::vector<Resource> loadResource(const QUrl& url, IlwisTypes types) const;

protected:
     IlwisObject *createObject(IlwisObjectConnector *connector,const IOOptions& options=IOOptions()) const;


};
}

#endif // ILWISOBJECTFACTORY_H
