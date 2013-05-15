#ifndef CATALOGQUERY_H
#define CATALOGQUERY_H

namespace Ilwis {
class CatalogQuery
{
public:
    CatalogQuery();
    QString transformQuery(const QString &baseQuery) const;
private:
    QHash<QString, quint64> _names;
};
}

#endif // CATALOGQUERY_H
