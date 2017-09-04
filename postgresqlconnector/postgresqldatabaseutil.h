#ifndef POSTGRESQLDATABASEUTIL_H
#define POSTGRESQLDATABASEUTIL_H

#include "kernel.h"
#include "resource.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "postgresqlparameters.h"

namespace Ilwis {

class SPFeatureI;
class FeatureCoverage;
class Table;

namespace Postgresql {

struct MetaGeometryColumn {
    QString catalog;
    QString schema = "public";
    QString tableName;
    QString geomColumn;
    int dimension;
    ICoordinateSystem crs;
    IlwisTypes geomType;
    QString qtablename() {
        QString qtablename = QString(schema);
        qtablename.append(".").append(tableName);
        return qtablename;
    }
};

struct MetaRasterColumn {
    QString catalog;
    QString schema = "public";
    QString tableName;
    QString rasterColumn;
    ICoordinateSystem crs;
    QString qtablename() {
        QString qtablename = QString(schema);
        qtablename.append(".").append(tableName);
        return qtablename;
    }
};

class PostgresqlDatabaseUtil {

public:

    PostgresqlDatabaseUtil(const PostgresqlParameters & params);
    ~PostgresqlDatabaseUtil();

    /**
     * @brief copyResourceWithProperties copies a resource by also copying the database
     * properties needed to access the database (user credentials, schema).
     * @return a copied instance
     */
    Resource resourceForType(IlwisTypes newType) const;

    void getMetaForRasterColumns(QList<MetaRasterColumn> &columns) const;

    void getMetaForGeometryColumns(QList<MetaGeometryColumn> &columns) const;

    bool exists(SPFeatureI feature) const;

    void getPrimaryKeys(QList<QString> &primaryColumns) const;

    void prepareSubFeatureSemantics(IDomain &domain, const QList<MetaGeometryColumn> &geomColumns, const IOOptions &options) const;

    QSqlQuery doQuery(QString stmt) const;

    bool tableExists() const;
    bool createTable(Ilwis::FeatureCoverage *fcoverage) const;
    bool createTable(Table *table) const;
private:
    void prepareCoordinateSystem(QString srid, ICoordinateSystem &crs) const;

    PostgresqlParameters _params;

    /**
     * @brief open creates a database connection, or re-uses a previously opened connection.
     * @return a database connection setup with the given user credentials.
     */
    QSqlDatabase open() const;

};

}
}
#endif // POSTGRESQLDATABASEUTIL_H
