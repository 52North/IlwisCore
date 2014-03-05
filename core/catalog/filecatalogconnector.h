#ifndef FILECATALOGCONNECTOR_H
#define FILECATALOGCONNECTOR_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT FileCatalogConnector : public CatalogConnector
{
public:
    static ConnectorInterface *create(const Resource &resource, bool load);
    QFileInfo toLocalFile(const QUrl &datasource) const;
    static  std::vector<QUrl> loadFolders(const Ilwis::Resource &source, const QStringList &namefilter, int options) ;

protected:
    FileCatalogConnector(const Resource &resource, bool load=true);
    bool prepare();

    bool canUse(const Resource &resource) const;

    std::vector<QUrl> sources(const QStringList &filters, int options =foFULLPATHS) const;
    virtual bool loadItems();
    QString provider() const;


private:

};
}

#endif // FILECATALOGCONNECTOR_H
