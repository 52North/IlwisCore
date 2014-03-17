#ifndef INTERNALCATALOGCONNECTOR_H
#define INTERNALCATALOGCONNECTOR_H

namespace Ilwis {
namespace Internal {
class InternalCatalogExplorer : public CatalogExplorer
{
public:
    InternalCatalogExplorer(const Ilwis::Resource &,const PrepareOptions& options=PrepareOptions());
    ~InternalCatalogExplorer();
    static CatalogExplorer *create(const Resource &,const PrepareOptions& options=PrepareOptions());

    QFileInfo toLocalFile(const QUrl &url) const;
    std::vector<Resource> loadItems();

    bool isValid() const;
    bool canUse(const Resource &resource) const;
    QString provider() const;

    NEW_CATALOGEXPLORER(InternalCatalogExplorer);
};
}
}

#endif // INTERNALCATALOGCONNECTOR_H
