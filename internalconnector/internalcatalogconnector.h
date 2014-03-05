#ifndef INTERNALCATALOGCONNECTOR_H
#define INTERNALCATALOGCONNECTOR_H

namespace Ilwis {
namespace Internal {
class InternalCatalogConnector : public CatalogConnector
{
public:
    InternalCatalogConnector(const Ilwis::Resource &, bool load=true);
    ~InternalCatalogConnector();
    static ConnectorInterface *create(const Resource &, bool);


    bool prepare();
    std::vector<QUrl> sources(const QStringList& filter, int options=foFULLPATHS) const;
    QFileInfo toLocalFile(const QUrl &url) const;
    bool loadItems();

    bool isValid() const;
    bool canUse(const Resource &resource) const;
    QString provider() const;

private:
    bool createItems(QSqlQuery &db, const QString &table, IlwisTypes type);
    bool createPcs(QSqlQuery &db);
    bool createSpecialDomains();
};
}
}

#endif // INTERNALCATALOGCONNECTOR_H
