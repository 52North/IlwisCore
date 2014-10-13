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

    bool checkForProperty(const std::vector<QString> &resourceBaseNames, QString &side, bool left, bool uselike) const;
};
}

#endif // CATALOGQUERY_H
