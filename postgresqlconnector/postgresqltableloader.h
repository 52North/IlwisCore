#ifndef POSTGRESQLTABLELOADER_H
#define POSTGRESQLTABLELOADER_H

namespace Ilwis {

class Resource;
class FeatureCoverage;
class RasterCoverage;

namespace Postgresql {


class PostgresqlTableLoader
{
public:
    PostgresqlTableLoader(const Resource &resource, const IOOptions &options);
    ~PostgresqlTableLoader();

    bool loadMetadata(Table *table) const;
    bool loadData(Table *table) const;

private:
    Resource _resource;
    IOOptions _options;

    QString select(QString columns) const;
    bool createColumnDefinition(Table *table, const QSqlQuery &query, QList<QString> &primaryKeys) const;
};

}
}
#endif // POSTGRESQLTABLELOADER_H
