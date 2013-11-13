#ifndef INTERNALCATALOGCONNECTOR_H
#define INTERNALCATALOGCONNECTOR_H

namespace Ilwis {
namespace Internal {
class InternalCatalogConnector : public ContainerConnector
{
public:
    InternalCatalogConnector();
    static ConnectorInterface *create(const Resource &, bool);
    bool loadItems();
    bool canUse(const Resource &resource) const;
    QString provider() const;

    bool prepare();
    std::vector<QUrl> sources(const QStringList& filter, int options=foFULLPATHS) const;
    QFileInfo toLocalFile(const QUrl &url) const;

    bool isValid() const;
private:
    bool createItems(QSqlQuery &db, const QString &table, IlwisTypes type);
    bool createPcs(QSqlQuery &db);
    bool createSpecialDomains();
};
}
}

#endif // INTERNALCATALOGCONNECTOR_H
