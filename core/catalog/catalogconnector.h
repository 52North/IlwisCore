#ifndef CATALOGCONNECTOR_H
#define CATALOGCONNECTOR_H

#include "kernel_global.h"
#include <memory>
#include <QStringList>

namespace Ilwis {

class Catalog;
class CatalogConnector;
class CatalogExplorer;
class MasterCatalogCache;

typedef std::unique_ptr<CatalogConnector> UPCatalogConnector;
typedef std::shared_ptr<CatalogExplorer> SPCatalogExplorer;

/**
 * The CatalogConnector is the only connector to connect to catalogs. As a catalog can be a hetregenous
 * collection of data-sources and other catalogs it is not possible to have one connector per data-source type as
 * is the case with other data connectors. Instead the catalog connector sub contracts the access to different types of
 * catalog sources to the catalog explorers. The catalog explorers are format specific and often are combined with the
 * data source explorer that is able to read a certain kind of data source. E.g. there is a CatalogExplorer dervative for
 * GdalConnector. This explorer is able to query the properties of data sources that are supported by gdal.
 * In this way the CatalogConnector exposes a format independent interface to the various collections of data sources
 * accessible by ilwis-objects.
 *
 */
class KERNELSHARED_EXPORT CatalogConnector : public IlwisObjectConnector
{
public:
    enum FilterOptions{ foFULLPATHS=1, foNAMESONLY=2, foEXTENSIONFILTER=4};
    enum OpenMode { omTEXT, omBINARY};


    /**
     * @brief creates a CatalogConnector object.
     *
     * @param resource
     * @param load
     * @param options options for loading items from the catalog
     */
    CatalogConnector(const Resource& resource, bool load, const IOOptions& options);
    /**
     * @brief creates a default version of the ilwisobject this connector is dedicated to, in this case a Catalog object.
     *
     * The returned object is just an ilwisobject with some properties set but
     * @return pointer to a an Ilwis::IlwisObject. The ownership of the object goes to the caller
     */
    Ilwis::IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());

    virtual ~CatalogConnector() {}

    QString provider() const;

    virtual bool canUse(const Resource& resource) const;
    bool isValid() const;

    bool loadMetaData(IlwisObject *obj, const IOOptions &options = IOOptions());
    bool loadData(IlwisObject *obj, const IOOptions &options = IOOptions());
    void unload();

    virtual QFileInfo toLocalFile(const Resource &datasource) const;
    virtual QFileInfo toLocalFile(const QUrl &url) const;

    static const std::unique_ptr<MasterCatalogCache>& cache();
    std::vector<QString> providers() const;



protected:
    virtual  bool loadExplorers();


private:
    std::vector<SPCatalogExplorer> _dataProviders;
    bool loadDataThreaded(IlwisObject *obj, const IOOptions &options);
    bool loadDataSingleThread(IlwisObject *obj, const IOOptions &options);
    static std::unique_ptr<MasterCatalogCache> _mcCache;
    std::mutex _load;
    void updateWorkflowResources(std::vector<Resource> &items);
};

}




#endif // CATALOGCONNECTOR_H
