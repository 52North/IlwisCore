#ifndef ILWIS3CATALOGCONNECTOR_H
#define ILWIS3CATALOGCONNECTOR_H

namespace Ilwis {

namespace Ilwis3 {

class Ilwis3CatalogExplorer : public FolderCatalogExplorer
{
public:
    Ilwis3CatalogExplorer(const Ilwis::Resource &resource,const IOOptions& options=IOOptions());

    std::vector<Resource> loadItems(const IOOptions &options=IOOptions());
    bool canUse(const Resource& resource) const;
    QString provider() const;

    static Ilwis::CatalogExplorer *create(const Ilwis::Resource &resource, const Ilwis::IOOptions &options);
    QFileInfo toLocalFile(const QUrl &datasource) const;
private:
    void registerNames(const QString &name, QHash<QString, quint64> &names);

    NEW_CATALOGEXPLORER(Ilwis3CatalogExplorer);
};
}
}

#endif // ILWIS3CATALOGCONNECTOR_H
