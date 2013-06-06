#ifndef INTERNALCATALOGCONNECTOR_H
#define INTERNALCATALOGCONNECTOR_H

namespace Ilwis {
namespace Internal {
class InternalCatalogConnector : public Ilwis::CatalogConnector
{
public:
    InternalCatalogConnector();
    static ConnectorInterface *create(const Resource &, bool);
    bool loadItems();
    bool canUse(const QUrl& res) const;

private:
    bool createItems(QSqlQuery &db, const QString &table, IlwisTypes type);
    bool createPcs(QSqlQuery &db);
    bool createSpecialDomains();
};
}
}

#endif // INTERNALCATALOGCONNECTOR_H
