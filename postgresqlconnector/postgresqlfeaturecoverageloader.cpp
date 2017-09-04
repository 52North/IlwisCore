#include <iostream>
#include <array>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>

#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coverage.h"
#include "domain.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "range.h"
#include "identifierrange.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "attributedefinition.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "geometryhelper.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"

#include "postgresqlconnector.h"
#include "postgresqlfeaturecoverageloader.h"
#include "postgresqltableconnector.h"
#include "postgresqltableloader.h"
#include "postgresqldatabaseutil.h"
#include "sqlstatementhelper.h"
#include "postgresqlparameters.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlFeatureCoverageLoader::PostgresqlFeatureCoverageLoader(const Resource &resource, const IOOptions &options): _resource(resource), _options(options)
{
}

PostgresqlFeatureCoverageLoader::~PostgresqlFeatureCoverageLoader()
{
    QSqlDatabase::removeDatabase("featurecoverageloader");
}

bool PostgresqlFeatureCoverageLoader::loadMetadata(FeatureCoverage *fcoverage) const
{
    //qDebug() << "PostgresqlFeatureCoverageLoader::loadMetadata()";

    ITable featureTable;
    if(!featureTable.prepare(_resource.url().toString(), itFLATTABLE, _options)) {
        ERROR1(ERR_NO_INITIALIZED_1, _resource.name() + "[itFLATTABLE]");
        return false;
    }

    setFeatureCount(fcoverage);
    setSpatialMetadata(fcoverage);
    fcoverage->setAttributes(featureTable);

    IDomain semantics;
    QList<MetaGeometryColumn> metaGeometries;
    PostgresqlParameters params (_resource.url(true).toString());
    PostgresqlDatabaseUtil pgUtil(params);
    pgUtil.getMetaForGeometryColumns(metaGeometries);
    pgUtil.prepareSubFeatureSemantics(semantics, metaGeometries, _options);
    setSubfeatureSemantics(fcoverage, semantics);

    return true;
}

void PostgresqlFeatureCoverageLoader::setSubfeatureSemantics(FeatureCoverage *fcoverage, IDomain &semantics) const
{
    std::vector<QString> items;
    ItemRangeIterator iter(semantics->range<>().data());
    while (iter.isValid()) {
        SPDomainItem geomName = (*iter);
        items.push_back(geomName->name());
        ++iter;
    }
    fcoverage->attributeDefinitionsRef().setSubDefinition(semantics, items);
}

QString PostgresqlFeatureCoverageLoader::selectGeometries(const QList<MetaGeometryColumn> &metaGeometry) const
{
    QString columns;
    std::for_each(metaGeometry.begin(), metaGeometry.end(), [&columns](MetaGeometryColumn meta) {
        columns.append(" ST_AsText(");
        columns.append(meta.geomColumn).append(") AS ");
        columns.append(meta.geomColumn).append(",");
    });
    columns = columns.left(columns.size() - 1);

    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(columns);
    sqlBuilder.append(" FROM ");

    PostgresqlParameters params (_resource.url(true).toString());
    sqlBuilder.append(params.schema() + "." + params.table());
    return sqlBuilder;
}

bool PostgresqlFeatureCoverageLoader::loadData(FeatureCoverage *fcoverage) const
{
    ITable table;
    PostgresqlParameters params (_resource.url(true).toString());
    PostgresqlDatabaseUtil pgUtil(params);
    Resource tableResource = pgUtil.resourceForType(itFLATTABLE);
    table.prepare(tableResource, _options);

    PostgresqlTableLoader tableLoader(table->resource(), _options);
    if (!tableLoader.loadData(table.ptr())) {
        ERROR1("Could not load table data for table '%1'", table->name());
        return false;
    }

    // metadata already set it to correct number, creating new features will up the count agains; so reset to 0.
    fcoverage->setFeatureCount(itFEATURE, iUNDEF, FeatureInfo::ALLFEATURES);

    QList<MetaGeometryColumn> metaGeometries;
    pgUtil.getMetaForGeometryColumns(metaGeometries);
    QString select = selectGeometries(metaGeometries);
    QSqlQuery query = pgUtil.doQuery(select);
    quint32 geometriesPerFeature = metaGeometries.size();
    IDomain semantics;
    pgUtil.prepareSubFeatureSemantics(semantics, metaGeometries, _options);

    while (query.next()) {
        if (geometriesPerFeature == 0) {
            fcoverage->newFeature(0);
        } else {
            // index 0 is root, indeces > 0 are subfeatures of root
            bool atRoot = true;
            SPFeatureI rootFeature;
            // iterate semantics to keep predefined order
            ItemRangeIterator iter(semantics->range<>().data());
            while (iter.isValid()) {
                QString geomName = (*iter)->name();
                ICoordinateSystem crs;
                std::for_each(metaGeometries.begin(), metaGeometries.end(), [&crs,geomName](MetaGeometryColumn c) {
                    if (c.geomColumn == geomName) {
                        crs = c.crs;
                    }
                });
                if (atRoot) {
                    atRoot = false;
                    geos::geom::Geometry *rootGeometry = createGeometry(query, geomName, crs);
                    rootFeature = fcoverage->newFeature(rootGeometry, false);
                } else {
                    geos::geom::Geometry *subGeometry = createGeometry(query, geomName, crs);
                    rootFeature->createSubFeature(geomName,subGeometry);
                }
                ++iter;
            }
        }
    }
    fcoverage->setAttributes(table);

    return true;
}

bool PostgresqlFeatureCoverageLoader::storeData(FeatureCoverage *fcoverage) const
{
    bool queryOk = true;
    ITable baseData = fcoverage->attributeTable();
    PostgresqlParameters params (_resource.url(true).toString());
    PostgresqlDatabaseUtil pgUtil(params);
    SqlStatementHelper sqlHelper(params);
   bool newTable = false;
   if ( !pgUtil.tableExists()){
       if(!pgUtil.createTable(fcoverage)){
           return false;
       }
       newTable = true;
   }

   IDomain semantics; // subfeature semantics
    QList<QString> primaryKeys; // readonly keys
    QList<MetaGeometryColumn> metaGeomColumns; // geometry columns
    pgUtil.getMetaForGeometryColumns(metaGeomColumns);
    pgUtil.prepareSubFeatureSemantics(semantics, metaGeomColumns, _options);
    pgUtil.getPrimaryKeys(primaryKeys);
    // add geoms to update/insert data table
    FeatureIterator featureIter(fcoverage);
    featureIter.flow(FeatureIterator::fDEPTHFIRST);

    QString columnNamesCommaSeparated = sqlHelper.columnNamesCommaSeparated(baseData.ptr());

    QString code = fcoverage->coordinateSystem()->code();
    QString srid = code.right(code.indexOf(":"));
    QString qtablename = params.table().toLower();
    while(featureIter != featureIter.end()) {
        SPFeatureI feature = (*featureIter);
        bool newFeature = newTable ? true : !pgUtil.exists(feature);
        QString columnValuesCommaSeparated = sqlHelper.columnValuesCommaSeparated(feature);
        QString sqlStmt;
        if (newFeature) {
            sqlStmt = "INSERT INTO ";
            sqlStmt.append(qtablename);


            sqlStmt.append(" ( ");
            sqlStmt.append(columnNamesCommaSeparated);

            foreach (MetaGeometryColumn geomMeta, metaGeomColumns) {
                sqlStmt.append(", ");
                sqlStmt.append(geomMeta.geomColumn);
            }

            sqlStmt.append(" ) ");
            sqlStmt.append(" VALUES ( ");
            sqlStmt.append(columnValuesCommaSeparated);
            sqlStmt.append(", ");

            // add geometry values

            /*
            // TODO storing level n data only makes sense
            //      if subfeatures are stored in separate
            //      tables related via foreign keys, but
            //      separate tables aren't supported now

            ITabel levelData = fcoverage->attributeTable(level);
            PostgresqlTableConnector connector(_resource,false);
            tableOk = connector.store(levelData.ptr(), options);
            */

            QString rootGeomColumn = fcoverage->attributeDefinitionsRef().index((quint32)0);
            // have to iterate the order of given geom columns
            foreach (MetaGeometryColumn geomMeta, metaGeomColumns) {
                QString wkt = "NULL";
                QString geomColumn = geomMeta.geomColumn;
                if (rootGeomColumn == geomColumn || rootGeomColumn == sUNDEF) {
                    if (feature->geometry() != nullptr){
                        wkt = QString("'%1'").arg(GeometryHelper::toWKT(feature->geometry().get()));
                    }
                } else if (feature[geomColumn]->geometry() != nullptr){
                    // access subfeature geometries
                    SPFeatureI subfeature = feature[geomColumn];
                    wkt = QString("'%1'").arg(GeometryHelper::toWKT(subfeature->geometry().get()));
                }
                sqlStmt.append("st_geomfromtext(").append(wkt);
                sqlStmt.append(", ").append(srid).append(")");
                sqlStmt.append(",");

                // skip subfeature's geometries, as accessed already via domain
                ++featureIter;
            }

            sqlStmt = sqlHelper.trimAndRemoveLastCharacter(sqlStmt);


            sqlStmt.append(" ); ");
        } else {

            // update feature

            sqlStmt = "UPDATE ";
            sqlStmt.append(qtablename);
            sqlStmt.append(" SET ");

            Record record = feature->record();
            for (int i = 0; i < feature->attributeColumnCount(); ++i) {
                ColumnDefinition coldef = feature->attributedefinition(i);
                sqlStmt.append(coldef.name()).append(" = ");
                sqlStmt.append(sqlHelper.createInsertValueString(record.cell(i), coldef));
                sqlStmt.append(", ");
            }

            quint32 levels = metaGeomColumns.size();
            for (quint32 level = 0 ; level < levels ; level++) {
                QString wkt = "NULL";
                if (feature->geometry() != nullptr) {
                    wkt = QString("'%1'").arg(GeometryHelper::toWKT(feature->geometry().get()));
                }

                QString geomColumn = fcoverage->attributeDefinitionsRef().index(level);
                sqlStmt.append(geomColumn).append(" = ");
                sqlStmt.append("st_geomfromtext(").append(wkt);
                sqlStmt.append(", ").append(srid).append(")");
                sqlStmt.append(",");

                ++featureIter; // next geom
                if (featureIter != featureIter.end()) {
                    feature = (*featureIter);
                }
            }

            if (metaGeomColumns.size() == 0) {
                ++featureIter; // next geom
                if (featureIter != featureIter.end()) {
                    feature = (*featureIter);
                }
            }

            sqlStmt = sqlHelper.trimAndRemoveLastCharacter(sqlStmt);
            QString where = " WHERE ";
            foreach (QString primaryKey, primaryKeys) {
                ColumnDefinition coldef = feature->attributedefinition(primaryKey);
                where.append(coldef.name()).append(" = ");
                QVariant value = record.cell(coldef.columnindex());
                where.append(sqlHelper.createInsertValueString(value, coldef));
                where.append(", ");
            }
            where = sqlHelper.trimAndRemoveLastCharacter(where);
            sqlStmt.append(" ").append(where);
            sqlStmt.append("; ");
        }

        pgUtil.doQuery(sqlStmt);
    }

    bool featuresOk = true;
    return queryOk && featuresOk;
}

geos::geom::Geometry* PostgresqlFeatureCoverageLoader::createGeometry(QSqlQuery &query, QString geomColumn, ICoordinateSystem crs) const
{
    // postgis wkb is different from ogc wkb
    // => select ewkb, but this seems to be slower than selecting wkt
    //ByteArray wkbBytes = variant.toByteArray();

    QVariant variant = query.value(geomColumn);
    QString wkt = variant.toString();
    return GeometryHelper::fromWKT(wkt,crs);
}


void PostgresqlFeatureCoverageLoader::setFeatureCount(FeatureCoverage *fcoverage) const
{
    //qDebug() << "PostgresqlFeatureCoverageLoader::setFeatureCount()";

    IDomain semantics;
    QList<MetaGeometryColumn> metaGeometries;
    PostgresqlParameters params (_resource.url(true).toString());
    PostgresqlDatabaseUtil pgUtil(params);
    pgUtil.getMetaForGeometryColumns(metaGeometries);
    pgUtil.prepareSubFeatureSemantics(semantics, metaGeometries, _options);

    int level = -1;
    QSqlQuery query;
    foreach (MetaGeometryColumn meta, metaGeometries) {
        QString sqlBuilder;
        sqlBuilder.append("SELECT  ");
        sqlBuilder.append(" count( * )");
        sqlBuilder.append(" AS count ");
        sqlBuilder.append(" FROM ");
        sqlBuilder.append(" (SELECT ");
        sqlBuilder.append(" * ");
        sqlBuilder.append(" FROM ");
        sqlBuilder.append(meta.qtablename());
        sqlBuilder.append(" WHERE NOT ");
        sqlBuilder.append(" ST_isEmpty( ");
        sqlBuilder.append(meta.geomColumn);
        sqlBuilder.append(" ) ");
        sqlBuilder.append(" ) AS not_null ;");
        //qDebug() << "SQL: " << sqlBuilder;

        query = pgUtil.doQuery(sqlBuilder);

        if (semantics.isValid()) {
            NamedIdentifierRange *range = semantics->range<NamedIdentifierRange>().data();
            for (int i = 0 ; i < range->count() ; i++) {
                SPDomainItem item = range->itemByOrder(i);
                if (item->name() == meta.geomColumn) {
                    level = i;
                    break;
                }
            }
        } else {
            level++; // first come first serve
        }

        if (query.next()) {
            IlwisTypes types = meta.geomType;
            int count = query.value("count").toInt();
            if (count > 0) {
                fcoverage->setFeatureCount(types, count, level);
            }
        }
    }
}

void PostgresqlFeatureCoverageLoader::setSpatialMetadata(FeatureCoverage *fcoverage) const
{
    //qDebug() << "PostgresqlFeatureCoverageLoader::setSpatialMetadata()";

    QList<MetaGeometryColumn> metaGeometries;
    PostgresqlParameters params (_resource.url(true).toString());
    PostgresqlDatabaseUtil pgUtil(params);
    pgUtil.getMetaForGeometryColumns(metaGeometries);

    Envelope bbox;
    ICoordinateSystem crs;

    foreach (MetaGeometryColumn meta, metaGeometries) {
        QString sqlBuilder;
        sqlBuilder.append("SELECT ");
        sqlBuilder.append("st_extent( ");
        sqlBuilder.append(meta.geomColumn);
        sqlBuilder.append(" ) ");
        sqlBuilder.append(" FROM ");
        sqlBuilder.append(meta.qtablename());
        sqlBuilder.append(";");
        //qDebug() << "SQL: " << sqlBuilder;

        QSqlQuery envelopeQuery = pgUtil.doQuery(sqlBuilder);

        if (envelopeQuery.next()) {

            // TODO check with Martin how to handle multiple
            // geometries for one entity
            QString envString = envelopeQuery.value(0).toString();
            if ( !envString.isEmpty()) {
                Envelope envelope(envString);
                bbox += envelope;
            }
        }

        if ( !crs.isValid() && meta.crs.isValid()) {
            // first valid srid found is being considered as "main" crs.
            //
            // note: if multiple geom columns do exist, the geometries have
            // to be transformed to the "main" one when actual data is loaded
            crs = meta.crs;
        }

        fcoverage->coordinateSystem(crs);
        fcoverage->envelope(bbox);
    }
}





