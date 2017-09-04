#ifndef ILWIS3CATALOGFILEEXPLORER_H
#define ILWIS3CATALOGFILEEXPLORER_H


namespace Ilwis{
namespace Ilwis3{
class Ilwis3CatalogFileExplorer : public CatalogExplorer
{
public:
    Ilwis3CatalogFileExplorer(const Ilwis::Resource &resource,const IOOptions& options=IOOptions());
    static Ilwis::CatalogExplorer *create(const Resource &resource,const IOOptions& options=IOOptions());

    bool canUse(const Resource& resource) const;

    std::vector<Ilwis::Resource> loadItems(const IOOptions &options=IOOptions());
    QString provider() const;

    QFileInfo toLocalFile(const QUrl &datasource) const;


    NEW_CATALOGEXPLORER(Ilwis3CatalogFileExplorer);

    CatalogExplorer::ExplorerType explorerType() const;
private:
};
}
}

#endif // ILWIS3CATALOGFILEEXPLORER_H
