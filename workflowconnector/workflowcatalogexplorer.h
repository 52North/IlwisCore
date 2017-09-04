#ifndef WorkflowCATALOGEXPLORER_H
#define WorkflowCATALOGEXPLORER_H

namespace Ilwis {
namespace WorkflowConnector {


class WorkflowCatalogExplorer : public CatalogExplorer
{
public:
    WorkflowCatalogExplorer(const Ilwis::Resource &resource,const IOOptions& options=IOOptions());

    std::vector<Ilwis::Resource> loadItems(const IOOptions &options=IOOptions());
    bool canUse(const Resource& resource) const;
    QString provider() const;
    virtual QFileInfo toLocalFile(const QUrl &datasource) const;

    static Ilwis::CatalogExplorer *create(const Resource &resource,const IOOptions& options=IOOptions());

    NEW_CATALOGEXPLORER(WorkflowCatalogExplorer);
};
}
}

#endif // WorkflowCATALOGEXPLORER_H
