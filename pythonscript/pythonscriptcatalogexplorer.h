#ifndef PYTHONSCRIPTCATALOGEXPLORER_H
#define PYTHONSCRIPTCATALOGEXPLORER_H

namespace Ilwis{
namespace PythonScript {
class PythonScriptCatalogExplorer : public CatalogExplorer
{
public:
    PythonScriptCatalogExplorer(const Ilwis::Resource &resource,const IOOptions& options=IOOptions());

    std::vector<Ilwis::Resource> loadItems(const IOOptions &options=IOOptions());
    bool canUse(const Resource& resource) const;
    QString provider() const;

    QFileInfo toLocalFile(const QUrl &datasource) const;

    static Ilwis::CatalogExplorer *create(const Resource &resource,const IOOptions& options=IOOptions());
private:
    NEW_CATALOGEXPLORER(PythonScriptCatalogExplorer);

};
}
}

#endif // PYTHONSCRIPTCATALOGEXPLORER_H
