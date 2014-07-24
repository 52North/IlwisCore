#ifndef CATALOGCONNECTOR_H
#define CATALOGCONNECTOR_H

#include "kernel_global.h"
#include <QStringList>

namespace Ilwis {

class Catalog;
class CatalogConnector;
class CatalogExplorer;

typedef std::unique_ptr<CatalogConnector> UPCatalogConnector;
typedef std::unique_ptr<CatalogExplorer> UPCatalogExplorer;

class KERNELSHARED_EXPORT CatalogConnector : public IlwisObjectConnector
{
public:
    enum FilterOptions{ foFULLPATHS=1, foNAMESONLY=2, foEXTENSIONFILTER=4};
    enum OpenMode { omTEXT, omBINARY};

    CatalogConnector(const Resource& resource, bool load);
    Ilwis::IlwisObject *create() const;

    virtual ~CatalogConnector() {}

    bool loadMetaData(IlwisObject *obj,const IOOptions&);
    bool loadData(IlwisObject *obj, const Ilwis::IOOptions &options = IOOptions());

    bool isValid() const;
    virtual bool canUse(const Resource& resource) const;

    virtual QFileInfo toLocalFile(const Resource &datasource) const;
    virtual QFileInfo toLocalFile(const QUrl &url) const;

    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());

protected:
    virtual  bool loadExplorers();


private:
    std::vector<UPCatalogExplorer> _dataProviders;
};

}


#endif // CATALOGCONNECTOR_H
