#ifndef SPREADSHEETCATALOGEXPLORER_H
#define SPREADSHEETCATALOGEXPLORER_H

namespace Ilwis {
namespace SpreadSheetConnectors {


class SpreadSheetCatalogExplorer : public CatalogExplorer
{
public:
    SpreadSheetCatalogExplorer(const Ilwis::Resource &resource,const IOOptions& options=IOOptions());

    std::vector<Ilwis::Resource> loadItems(const IOOptions &options=IOOptions());
    bool canUse(const Resource& resource) const;
    QString provider() const;

    QFileInfo toLocalFile(const QUrl &datasource) const;


    static Ilwis::CatalogExplorer *create(const Resource &resource,const IOOptions& options=IOOptions());

    NEW_CATALOGEXPLORER(SpreadSheetCatalogExplorer);
};
}
}

#endif // SPREADSHEETCATALOGEXPLORER_H
