#include <QSqlDatabase>

#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "resource.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogexplorer.h"
#include "catalog.h"

#include "postgresqldatabaseutil.h"
#include "postgresqlcatalogexplorer.h"
#include "postgresqlparameters.h"

using namespace Ilwis;
using namespace Postgresql;

REGISTER_CATALOGEXPLORER(PostgresqlCatalogExplorer)

CatalogExplorer *PostgresqlCatalogExplorer::create(const Resource &resource, const IOOptions &options)
{
    return new PostgresqlCatalogExplorer(resource, options);
}

PostgresqlCatalogExplorer::PostgresqlCatalogExplorer(const Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{
    PostgresqlParameters params (resource.url(true).toString());
    if (params.valid())
        params.toIOOptions(ioOptionsRef());
}

PostgresqlCatalogExplorer::~PostgresqlCatalogExplorer()
{
}

IOOptions PostgresqlCatalogExplorer::merge(const IOOptions &first, const IOOptions &second) {

    IOOptions result;

    auto end = first.end();

    for (auto it = first.begin(); it != end; ++it)
    {
        result.addOption(  it.key(), it.value());
    }

    end = second.end();
    for (auto it = second.begin(); it != end; ++it)
    {
        result.addOption(  it.key(), it.value());
    }

    return result;
}

std::vector<Resource> PostgresqlCatalogExplorer::loadItems(const IOOptions &options)
{
    IOOptions iooptions = this->ioOptions();
    iooptions = merge(iooptions, options);
    //IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    PostgresqlParameters params (iooptions);
    std::vector<Resource> resources;
    QString sqlBuilder;
    if (!params.database().isEmpty()) {
        if (!params.schema().isEmpty()) {
            if (!params.table().isEmpty()) { // enumerate raster records in multiraster table, adds one rastercoverage for each record
                PostgresqlDatabaseUtil pgUtil(params);
                sqlBuilder = "SELECT rid FROM " + params.schema() + "." + params.table() + " WHERE " + (params.column().isEmpty() ? "rast" : params.column()) + " IS NOT NULL";
                qDebug() << "SQL: " << sqlBuilder;
                QSqlQuery query = pgUtil.doQuery(sqlBuilder);
                while (query.next()) {
                    QString id = query.value(0).toString();
                    params.setRasterID(id);
                    QString resourceId = params.toString();
                    QUrl url(resourceId);
                    Resource rastertable(url, itRASTER);
                    rastertable.setExtendedType(itFLATTABLE);
                    resources.push_back(rastertable);
                }
            } else { // discover tables
                PostgresqlDatabaseUtil pgUtil(params);
                // add as whole-table FeatureCoverages
                sqlBuilder = "SELECT DISTINCT table_name FROM INFORMATION_SCHEMA.Columns WHERE table_schema='" + params.schema() + "' and udt_name='geometry'";
                qDebug() << "SQL: " << sqlBuilder;
                QSqlQuery query = pgUtil.doQuery(sqlBuilder);
                while (query.next()) {
                    QString tablename = query.value(0).toString();
                    params.setTable(tablename);
                    QString resourceId = params.toString();
                    QUrl url(resourceId);
                    Resource featuretable(url, itFEATURE);
                    featuretable.setExtendedType(itFLATTABLE);
                    resources.push_back(featuretable);
                }

                // add as whole-table RasterCoverages
                sqlBuilder = "SELECT DISTINCT table_name FROM INFORMATION_SCHEMA.Columns WHERE table_schema='" + params.schema() + "' and udt_name='raster'";
                qDebug() << "SQL: " << sqlBuilder;
                query = pgUtil.doQuery(sqlBuilder);
                while (query.next()) {
                    QString tablename = query.value(0).toString();
                    params.setTable(tablename);
                    QString resourceId = params.toString();
                    QUrl url(resourceId);
                    Resource rastertable(url, itRASTER);
                    rastertable.setExtendedType(itFLATTABLE);
                    resources.push_back(rastertable);
                }

                // add per-geometry-column FeatureCoverages
                sqlBuilder = "SELECT table_name,column_name FROM INFORMATION_SCHEMA.Columns WHERE table_schema='" + params.schema() + "' and udt_name='geometry'";
                query = pgUtil.doQuery(sqlBuilder);
                while (query.next()) {
                    QString tablename = query.value(0).toString();
                    QString columnname = query.value(1).toString();
                    params.setTable(tablename);
                    params.setColumn(columnname);
                    QString resourceId = params.toString();
                    QUrl url(resourceId);
                    Resource featuretable(url, itFEATURE);
                    featuretable.setExtendedType(itFLATTABLE);
                    resources.push_back(featuretable);
                }

                // add per-raster-column RasterCoverages
                sqlBuilder = "SELECT table_name,column_name FROM INFORMATION_SCHEMA.Columns WHERE table_schema='" + params.schema() + "' and udt_name='raster'";
                qDebug() << "SQL: " << sqlBuilder;
                query = pgUtil.doQuery(sqlBuilder);
                while (query.next()) {
                    QString tablename = query.value(0).toString();
                    QString columnname = query.value(1).toString();
                    params.setTable(tablename);
                    params.setColumn(columnname);
                    QString resourceId = params.toString();
                    QUrl url(resourceId);
                    // Probe whether we're dealing with a single-raster-table or a multiraster table whereby each record is one raster; Note that for performance reasons we only check the first 100 records.
                    sqlBuilder = "WITH refrast AS (SELECT " + columnname + " AS rast FROM " + params.schema() + "." + tablename + " WHERE " + columnname + " IS NOT NULL AND ST_ScaleX(" + columnname + ") != 0 AND ST_ScaleY(" + columnname + ") != 0 LIMIT 1), crossrast AS (SELECT " + columnname + " AS rast FROM " + params.schema() + "." + tablename + " WHERE " + columnname + " IS NOT NULL AND ST_ScaleX(" + columnname + ") != 0 AND ST_ScaleY(" + columnname + ") != 0 OFFSET 1 LIMIT 100), conditions AS (SELECT ST_SameAlignment(crossrast.rast,refrast.rast) AS same, (ST_UpperLeftX(refrast.rast) / ST_ScaleX(refrast.rast) - ST_UpperLeftX(crossrast.rast) / ST_ScaleX(crossrast.rast))::integer % ST_Width(refrast.rast) AS modX, (ST_UpperLeftY(refrast.rast) / ST_ScaleY(refrast.rast) - ST_UpperLeftY(crossrast.rast) / ST_ScaleY(crossrast.rast))::integer % ST_Height(refrast.rast) AS modY, ST_Touches(ST_Envelope(crossrast.rast), ST_Envelope(refrast.rast)) OR NOT ST_Intersects(ST_Envelope(crossrast.rast), ST_Envelope(refrast.rast)) AS disjoint FROM crossrast, refrast GROUP BY same, modX, modY, disjoint) SELECT same AND modX=0 AND modY=0 AND disjoint AS singleraster FROM conditions GROUP BY singleraster ORDER BY singleraster";
                    QSqlQuery query2 = pgUtil.doQuery(sqlBuilder);
                    if (query2.next()) {
                        bool fSingleRaster = query2.value(0).toBool();
                        if (fSingleRaster) { // Officially we should test on same_alignment and regular_blocking, however currently (postgis 2.x / AddRasterConstraints may not have been called) the result of above query is more reliable than "SELECT scale_x, scale_y, blocksize_x, blocksize_y, extent FROM raster_columns WHERE r_table_schema='schema' AND r_table_name='tablename' AND r_raster_column='columnname'"
                            Resource rastertable(url, itRASTER);
                            rastertable.setExtendedType(itFLATTABLE);
                            resources.push_back(rastertable);
                        } else {
                            Resource rastercatalog(url, itCATALOG);
                            resources.push_back(rastercatalog);
                        }
                    } else { // There are 0 or 1 records in the results; probe again.
                        sqlBuilder = "SELECT COUNT(" + columnname + ") FROM " + params.schema() + "." + tablename + " WHERE " + columnname + " IS NOT NULL";
                        query2 = pgUtil.doQuery(sqlBuilder);
                        if (query2.next()) {
                            qlonglong count = query2.value(0).toLongLong();
                            if (count == 1) { // Table with a single raster record; we treat the whole table as a single raster (is that a good decision?)
                                Resource rastertable(url, itRASTER);
                                rastertable.setExtendedType(itFLATTABLE);
                                resources.push_back(rastertable);
                            } else if (count > 1) {
                                Resource rastercatalog(url, itCATALOG);
                                resources.push_back(rastercatalog);
                            }
                        }
                    }
                }

                // add the remaining tables (non-geometry and non-raster) as plain tables
                params.setColumn("");
                sqlBuilder = "WITH geoms AS (SELECT DISTINCT table_name FROM INFORMATION_SCHEMA.Columns WHERE table_schema='" + params.schema() + "' AND (udt_name = 'geometry' OR udt_name = 'raster')) SELECT DISTINCT table_name FROM INFORMATION_SCHEMA.Columns WHERE table_schema='" + params.schema() + "' AND table_name NOT IN (SELECT * FROM geoms)";
                query = pgUtil.doQuery(sqlBuilder);
                while (query.next()) {
                    QString tablename = query.value(0).toString();
                    params.setTable(tablename);
                    QString resourceId = params.toString();
                    QUrl url(resourceId);
                    Resource plaintable(url, itFLATTABLE);
                    resources.push_back(plaintable);
                }
            }
        } else { // discover schemas
            sqlBuilder = "SELECT distinct table_schema FROM INFORMATION_SCHEMA.Columns";
            PostgresqlDatabaseUtil pgUtil(params);
            QSqlQuery query = pgUtil.doQuery(sqlBuilder);
            while (query.next()) {
                QString schemaname = query.value(0).toString();
                params.setSchema(schemaname);
                QString resourceId = params.toString();
                QUrl url(resourceId);
                Resource schema(url, itCATALOG);
                resources.push_back(schema);
            }
        }
    } else { // discover databases
        sqlBuilder = "SELECT datname FROM pg_database WHERE NOT datistemplate order by datname;";
        params.setDatabase("postgres"); // the maintenance database where all other databases are registered
        PostgresqlDatabaseUtil pgUtil(params);
        QSqlQuery query = pgUtil.doQuery(sqlBuilder);
        while (query.next()) {
            QString databasename = query.value(0).toString();
            params.setDatabase(databasename);
            QString resourceId = params.toString();
            QUrl url(resourceId);
            Resource database(url, itCATALOG);
            resources.push_back(database);
        }
    }
    return resources;
}

bool PostgresqlCatalogExplorer::canUse(const Resource &resource) const
{
    if (resource.url().scheme() != "postgresql")
        return false;
    if ( resource.ilwisType() != itCATALOG)
        return false;
    PostgresqlParameters params (resource.url(true).toString());
    return params.valid();
}

QString PostgresqlCatalogExplorer::provider() const
{
    return "postgresql";
}

