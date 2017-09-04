#ifndef SpreadSheetCatalogFileExplorer_H
#define SpreadSheetCatalogFileExplorer_H

namespace Ilwis {
namespace SpreadSheetConnectors {
class SpreadSheetCatalogFileExplorer : public CatalogExplorer
{
public:
    SpreadSheetCatalogFileExplorer();

    SpreadSheetCatalogFileExplorer(const Ilwis::Resource &resource,const IOOptions& options=IOOptions());
    static Ilwis::CatalogExplorer *create(const Resource &resource,const IOOptions& options=IOOptions());

    bool canUse(const Resource& resource) const;

    std::vector<Ilwis::Resource> loadItems(const IOOptions &options=IOOptions());
    QString provider() const;

    QFileInfo toLocalFile(const QUrl &datasource) const;


    NEW_CATALOGEXPLORER(SpreadSheetCatalogFileExplorer);
    CatalogExplorer::ExplorerType explorerType() const;
};
}
}

#endif // SpreadSheetCatalogFileExplorer_H
