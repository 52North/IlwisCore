#ifndef ABSTRACTFACTORY_H
#define ABSTRACTFACTORY_H

#include "kernel_global.h"




namespace Ilwis {

class Resource;
class CatalogConnector;
typedef std::unique_ptr<Ilwis::CatalogConnector> UPCatalogConnector;

/*!
 * \brief The AbstractFactory class implements some of the more general methods that are in de FactoryInterface
 */
class KERNELSHARED_EXPORT AbstractFactory : public FactoryInterface
{
public:
    /*!
     returns an optional description of the functionality of the factory
     * \return
     */
    QString description() const;
    /*!
    return the type of the factory. The combination, type+subtype (\se AbstractFactory::key) is used to locate factories in the master-factory
     * \return
     */
    QString type() const;
    /*!
     while the type describes the general type, the subtype usualy points to the plug-in were it can be located. E.g. there is an IlwisObjectFactory in
     both the Ilwis3 connector plugin as in the Gdal connector plugin. The subtype "ilwis3" and "gdal" discriminates between them
     * \return the subtype of the factory
     */
    QString subtype() const;
    /*!
     The combination of subtype and type in the format <subtype>::<type>, e.g. ilwis3::ilwisobjectfactory. Keys are always in lower case.
     * \return a string with the unique key for this factory
     */
    QString key() const;
    /*!
    initialization of the factory. The method is optional as not all factories need it.
     * \return true if succesfull. Any errors can be found on the issuelog
     */
    bool prepare() ;
    /*!
     checks if a resource can be used by this factory. The method is optional as not all factories do need it.
     * \param resource
     * \return
     */
    bool canUse(const Resource &resource) const;
protected:
    AbstractFactory(const QString& ty, const QString& sub, const QString& desc="");
private:

    QString _type;
    QString _subtype;
    QString _description;

};
}

typedef bool (* CheckUsage)(const Ilwis::Resource&,const Ilwis::UPCatalogConnector &);

#endif // ABSTRACTFACTORY_H
