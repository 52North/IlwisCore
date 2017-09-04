#ifndef StreamCatalogExplorer_H
#define StreamCatalogExplorer_H

#include <QNetworkAccessManager>

namespace Ilwis {
namespace Stream {
class StreamCatalogExplorer : public CatalogExplorer
{
public:
    StreamCatalogExplorer(const Ilwis::Resource &resource,const IOOptions& options=IOOptions());

    std::vector<Resource> loadItems(const Ilwis::IOOptions &options=IOOptions());
    bool canUse(const Resource& resource) const;
    QString provider() const;

    static Ilwis::CatalogExplorer *create(const Ilwis::Resource &resource, const Ilwis::IOOptions &options);

private:
    QFileInfo toLocalFile(const QUrl &datasource) const { return QFileInfo();}


    NEW_CATALOGEXPLORER(StreamCatalogExplorer);
    IlwisObject *createType(IlwisTypes tp);
};
}
}

#endif // StreamCatalogExplorer_H
