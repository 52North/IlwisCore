#ifndef FolderCatalogExplorer_H
#define FolderCatalogExplorer_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT FolderCatalogExplorer : public CatalogExplorer
{
public:
    static CatalogExplorer *create(const Resource &resource, bool load,const PrepareOptions& options=PrepareOptions());
    QFileInfo toLocalFile(const QUrl &datasource) const;
    static  std::vector<QUrl> loadFolders(const Ilwis::Resource &source, const QStringList &namefilter, int options) ;

protected:
    FolderCatalogExplorer(const Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());

    bool canUse(const Resource &resource) const;
    QString provider() const;
    std::vector<Resource> loadItems();

    std::vector<QUrl> sources(const QStringList &filters, int options =CatalogConnector::foFULLPATHS) const;

private:

};
}

#endif // FolderCatalogExplorer_H
