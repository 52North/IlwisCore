#ifndef INTERNALCATALOGCONNECTOR_H
#define INTERNALCATALOGCONNECTOR_H

namespace Ilwis {
namespace Internal {
class InternalCatalogExplorer : public CatalogExplorer
{
public:
    InternalCatalogExplorer(const Ilwis::Resource &,const IOOptions& options=IOOptions());
    ~InternalCatalogExplorer();
    static CatalogExplorer *create(const Resource &,const IOOptions& options=IOOptions());

    QFileInfo toLocalFile(const QUrl &url) const;
    std::vector<Resource> loadItems(const IOOptions& options=IOOptions());

    bool isValid() const;
    bool canUse(const Resource &resource) const;
    QString provider() const;

    NEW_CATALOGEXPLORER(InternalCatalogExplorer);
};
}
}

#endif // INTERNALCATALOGCONNECTOR_H
