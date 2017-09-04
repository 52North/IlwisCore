#ifndef REMOTECATALOGEXPLORER_H
#define REMOTECATALOGEXPLORER_H

#include <QNetworkAccessManager>

namespace Ilwis {
namespace Stream {
class RemoteCatalogExplorer : public CatalogExplorer
{
public:
    RemoteCatalogExplorer(const Ilwis::Resource &resource,const IOOptions& options=IOOptions());

    std::vector<Resource> loadItems(const Ilwis::IOOptions &options=IOOptions());
    bool canUse(const Resource& resource) const;
    QString provider() const;

    static Ilwis::CatalogExplorer *create(const Ilwis::Resource &resource, const Ilwis::IOOptions &options);

private:
    QFileInfo toLocalFile(const QUrl &datasource) const { return QFileInfo();}

    QNetworkAccessManager _manager;

    NEW_CATALOGEXPLORER(RemoteCatalogExplorer);
};
}
}

#endif // REMOTECATALOGEXPLORER_H
