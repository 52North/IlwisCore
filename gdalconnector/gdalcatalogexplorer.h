#ifndef GDALCATALOGCONNECTOR_H
#define GDALCATALOGCONNECTOR_H

namespace Ilwis{
namespace Gdal{
class GdalCatalogExplorer : public CatalogExplorer
{
public:
    GdalCatalogExplorer(const Ilwis::Resource &resource,const IOOptions& options=IOOptions());

    std::vector<Ilwis::Resource> loadItems(const IOOptions &options=IOOptions());
    bool canUse(const Resource& resource) const;
    QString provider() const;

   // std::vector<QUrl> sources(const Ilwis::Resource &resource, const QStringList& filter, int options=CatalogConnector::foFULLPATHS) const;
    QFileInfo toLocalFile(const QUrl &datasource) const;


    static Ilwis::CatalogExplorer *create(const Resource &resource,const IOOptions& options=IOOptions());
protected:
    IlwisTypes _type;

private:
    void getTypes(const std::multimap<QString, DataFormat> &formats, const QString &ext, IlwisTypes & tp, IlwisTypes & extendedType) const;

    NEW_CATALOGEXPLORER(GdalCatalogExplorer);
};
}
}

#endif // GDALCATALOGCONNECTOR_H
