#ifndef GDALCATALOGFILEEXPLORER_H
#define GDALCATALOGFILEEXPLORER_H

namespace Ilwis{
namespace Gdal{
class GdalCatalogFileExplorer : public CatalogExplorer
{
public:
    GdalCatalogFileExplorer(const Ilwis::Resource &resource,const IOOptions& options=IOOptions());
    static Ilwis::CatalogExplorer *create(const Resource &resource,const IOOptions& options=IOOptions());

    bool canUse(const Resource& resource) const;

    std::vector<Ilwis::Resource> loadItems(const IOOptions &options=IOOptions());
    QString provider() const;

    QFileInfo toLocalFile(const QUrl &datasource) const;


    NEW_CATALOGEXPLORER(GdalCatalogFileExplorer);

    CatalogExplorer::ExplorerType explorerType() const;
private:
    void getTypes(const std::multimap<QString, DataFormat> &formats, const QString &ext, IlwisTypes & tp, IlwisTypes & extendedType) const;
};
}
}

#endif // GDALCATALOGFILEEXPLORER_H
