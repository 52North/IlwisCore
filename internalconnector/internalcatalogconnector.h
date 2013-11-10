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
    QStringList sources(const QString& filter, int options=foFULLPATHS) const;
private:
    bool createItems(QSqlQuery &db, const QString &table, IlwisTypes type);
    bool createPcs(QSqlQuery &db);
    bool createSpecialDomains();
};
}
}

#endif // INTERNALCATALOGCONNECTOR_H
