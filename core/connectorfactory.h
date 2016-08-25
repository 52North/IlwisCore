#ifndef CONNECTORFACTORY_H
#define CONNECTORFACTORY_H

#include "kernel_global.h"
#include <QScopedPointer>
#include <QHash>

namespace Ilwis {

class ConnectorInterface;
class CatalogExplorer;

typedef std::function<Ilwis::CatalogExplorer*(const Ilwis::Resource &resource,const IOOptions& options)> createCatalogExplorer;

//typedef ConnectorInterface* (*ConnectorCreate)(const IlwisResource &resource);
//-----------------------------------------------------------------------------------------
struct ConnectorFilter{
    ConnectorFilter(quint64 types, const QString& provider) : _objectTypes(types), _provider(provider.toLower()) {}
    quint64 _objectTypes;
    QString _provider;
};

struct ConnectorFormatSelector{
    ConnectorFormatSelector(const QString& format, const QString& provider=sUNDEF) : _format(format.toLower()), _provider(provider.toLower()) {}

    QString _format;
    QString _provider;

};

KERNELSHARED_EXPORT uint qHash(const ConnectorFilter& resource );
KERNELSHARED_EXPORT bool operator==(const ConnectorFilter& filter1, const ConnectorFilter& filter2 );
KERNELSHARED_EXPORT bool operator<(const ConnectorFilter& filter1, const ConnectorFilter& filter2 );

KERNELSHARED_EXPORT uint qHash(const ConnectorFormatSelector& resource );
KERNELSHARED_EXPORT bool operator==(const ConnectorFormatSelector& filter1, const ConnectorFormatSelector& filter2 );
KERNELSHARED_EXPORT bool operator<(const ConnectorFormatSelector& filter1, const ConnectorFormatSelector& filter2 );


/*!
  This class creates a connector for a certain resource
  Connectors (implementations of the connector interface) exist in many forms in Ilwis. They are usually tied to specific type of format. The factory
  produces a specific implementation of the interface. It does this by applying a filter on the set of known connectors to find a suitable create function
  for a specific type of resource. Connectors can be created based on a format or purely on the resource.
 */
class KERNELSHARED_EXPORT ConnectorFactory : public AbstractFactory
{
    friend class ConnectorInterface;

public:
    /*!
     * \brief ConnectorFactory constructor for the factory
     */
    ConnectorFactory();

    /*!
     * \brief Each connector must implement a create method (static method) that can be passed to this factory to be available to the system.
     *
     * \param objecttypes a create function creates an connector that creates at least one type of objects but it may create more types. This parameter is OR combination of all the types it might create
     * \param provider the search key that is used to locate a particular create method
     */
    void addCreator(quint64 objecttypes, const QString &provider, ConnectorCreate);
    /*!
     * \brief Each connector must implement a create method (static method) that can be passed to this factory to be available to the system.
     * \param format
     * \param provider
     */
    void addCreator(const QString& format, const QString &provider, ConnectorCreate);

    /*!
     * Creates a connector based on the properties of the resource. The method will try to find a create function based on the type of the resource and the provider
     This method is usually called from objectfactories in a plug-in that implements a defined set of connectors. At that place the name of the provider is known and can be used.
     outside the plug-in the name of the provider is often not known and can not be used.
     * \param resource a resource of a certain type. To work the type must have a valid IlwisType
     * \param provider an identifier identifying the connector type
     * \return an instantiation of a connector or 0. In the case of 0 the error can be found in the issuelogger
     */

    template<class T=ConnectorInterface> T *createFromResource(const Resource& resource, const QString &provider,const IOOptions& options=IOOptions()) const{
        ConnectorFilter filter(resource.ilwisType(), provider);
        auto iter = _creatorsPerObject.find(filter);
        if ( iter == _creatorsPerObject.end())
            return 0;
        ConnectorCreate createConnector = iter.value();
        if ( createConnector ) {
            ConnectorInterface *cif =  createConnector(resource, true, options);
            if ( cif && cif->canUse(resource))
                return dynamic_cast<T *>(cif);
            delete cif;
        }
        kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
        return 0;

    }

    template<class T=ConnectorInterface> QList<T*> connectorsForResource(const Resource& resource, const QString &provider="*",const IOOptions& options=IOOptions()) const{
        ConnectorFilter filter(resource.ilwisType(), provider);
        QList<T*> results;
        for(auto key : _creatorsPerObject.keys()){
            if ( filter == key) {
                ConnectorCreate createConnector = _creatorsPerObject[key];
                if ( createConnector ) {
                    ConnectorInterface *cif =  createConnector(resource, true, options);
                    if ( cif && cif->canUse(resource)){
                        T* conn =  dynamic_cast<T *>(cif);
                        if ( conn)
                            results.push_back(conn);
                    }else
                        delete cif;
                }
            }
        }
        return results;

    }

    /*!
     * Creates a connector based on the properties of the resource. The method will try to find a create function based on the type of the resource and the format
     This method is usually called when a object must be connected to a specific format. For example when a script forces the use of a certain output format.
     * \param resource a resource of a certain type. To work the type must have a valid IlwisType
     * \return an instantiation of a connector or 0. In the case of 0 the error can be found in the issuelogger
     */
    template<class T=ConnectorInterface> T *createFromFormat(const Resource& resource, const QString &format, const QString& provider=sUNDEF,const IOOptions& options=IOOptions()) const{
        ConnectorFormatSelector filter(format, provider);
        auto iter = _creatorsPerFormat.find(filter);
        if ( iter == _creatorsPerFormat.end())
            return 0;
        ConnectorCreate createConnector = iter.value();
        if ( createConnector ) {
            ConnectorInterface *cif =  createConnector(resource, false, options);
            if ( cif){
                cif->format(format);
                return dynamic_cast<T *>(cif);
            }
        }


        kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
        return 0;
    }

    template<class T=ConnectorInterface> T *createContainerConnector(const Resource& resource, const QString &provider=sUNDEF,const IOOptions& options=IOOptions()) const{
        for(auto iter=_creatorsPerObject.begin(); iter != _creatorsPerObject.end(); ++iter){
            if ( !hasType(iter.key()._objectTypes, resource.ilwisType()))
                continue;
            ConnectorCreate createConnector = iter.value();
            if ( createConnector && ( iter.key()._provider == provider || provider == sUNDEF)) {
                ConnectorInterface *cif =  createConnector(resource, true, options);
                if ( cif && cif->canUse(resource))
                    return dynamic_cast<T *>(cif);
                delete cif;
            }
        }
        kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
        return 0;

    }

     std::vector<CatalogExplorer*> explorersForResource(const Resource& resource, const QString &provider=sUNDEF) const;

    static std::nullptr_t registerCatalogExplorer(createCatalogExplorer func);

protected:
    QHash<ConnectorFilter, ConnectorCreate > _creatorsPerObject;
    QHash<ConnectorFormatSelector, ConnectorCreate > _creatorsPerFormat;
    static std::vector<createCatalogExplorer> _explorers;
};
}


#endif // CONNECTORFACTORY_H
