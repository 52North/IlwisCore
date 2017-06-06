#ifndef PYTHONSCRIPTCATALOGEXPLORER_H
#define PYTHONSCRIPTCATALOGEXPLORER_H

namespace Ilwis{
class OperationResource;
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

    std::vector<Resource> loadOperationResource(QFile &file, const QString &filename, quint64 id);

    bool splitLine(QString line, QString &front, QString &back, bool toLower=false);
};
}
}

#endif // PYTHONSCRIPTCATALOGEXPLORER_H
