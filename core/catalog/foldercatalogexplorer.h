#ifndef FolderCatalogExplorer_H
#define FolderCatalogExplorer_H

#include "kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT FolderCatalogExplorer : public CatalogExplorer
{
public:
    static CatalogExplorer *create(const Resource &resource, bool load,const IOOptions& options=IOOptions());
    QFileInfo toLocalFile(const QUrl &datasource) const;
    static  std::vector<QUrl> loadFolders(const Ilwis::Resource &source, const QStringList &namefilter, int options) ;

protected:
    FolderCatalogExplorer(const Resource &resource, bool load=true,const IOOptions& options=IOOptions());

    bool canUse(const Resource &resource) const;
    QString provider() const;
    std::vector<Resource> loadItems(const IOOptions& options=IOOptions());

    std::vector<QUrl> sources(const QStringList &filters, int options =CatalogConnector::foFULLPATHS) const;

private:

};
}

#endif // FolderCatalogExplorer_H
