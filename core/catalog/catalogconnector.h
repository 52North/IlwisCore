#ifndef CATALOGCONNECTOR_H
#define CATALOGCONNECTOR_H

#include "Kernel_global.h"
#include <QStringList>

namespace Ilwis {

class Catalog;
class CatalogConnector;

typedef std::unique_ptr<CatalogConnector> UPCatalogConnector;

class KERNELSHARED_EXPORT CatalogConnector : public IlwisObjectConnector
{
public:
    enum FilterOptions{ foFULLPATHS=1, foNAMESONLY=2, foEXTENSIONFILTER=4};
    enum OpenMode { omTEXT, omBINARY};

    virtual ~CatalogConnector() {}

    Ilwis::IlwisObject *create() const;
    virtual  bool prepare();

    bool isValid() const;
    virtual bool canUse(const Resource& resource) const = 0;

    virtual bool loadItems() = 0;
    virtual QStringList itemlist() const {return QStringList();}
    virtual std::vector<QUrl> sources(const QStringList& filter, int options=foFULLPATHS) const = 0;

    virtual QFileInfo toLocalFile(const QUrl& datasource) const = 0;

protected:
    CatalogConnector(const Resource& resource, bool load);


private:
    std::vector<std::unique_ptr<CatalogConnector>> _dataProviders;

};

}

#endif // CATALOGCONNECTOR_H
