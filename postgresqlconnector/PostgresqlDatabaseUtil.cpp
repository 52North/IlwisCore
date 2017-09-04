#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>
#include "kernel.h"
#include "resource.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "conventionalcoordinatesystem.h"
#include "proj4parameters.h"
#include "domain.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "range.h"
#include "identifierrange.h"
#include "coverage.h"
#include "table.h"
#include "featurecoverage.h"
#include "rastercoverage.h"
#include "feature.h"
#include "raster.h"
#include "record.h"

#include "postgresqldatabaseutil.h"
#include "sqlstatementhelper.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlDatabaseUtil::PostgresqlDatabaseUtil(const PostgresqlParameters & params)
: _params(params)
{
}

PostgresqlDatabaseUtil::~PostgresqlDatabaseUtil()
{
}

QSqlDatabase PostgresqlDatabaseUtil::open() const
{
    QString connectionname = _params.toStringBasics();
    QSqlDatabase db;
    if (QSqlDatabase::contains(connectionname)) {
       db = QSqlDatabase::database(connectionname); // re-use previous connection to same user/pass/host/port/database
    } else {
       db = QSqlDatabase::addDatabase("QPSQL", connectionname);
       db.setHostName(_params.host());
       db.setPort(_params.port());
       db.setDatabaseName(_params.database());
       db.setUserName(_params.user());
       db.setPassword(_params.pass());
    }

    if ( !db.open()) {
       QString error = db.lastError().text();
       QString connection = _params.toString();
       ERROR2("Cannot establish connection to %1 (%2)", connection, error);
    }
    return db;
}

Resource PostgresqlDatabaseUtil::resourceForType(IlwisTypes newType) const
{
    return Resource(QUrl(_params.toString()), newType);
}

void Ilwis::Postgresql::PostgresqlDatabaseUtil::getMetaForRasterColumns(QList<Ilwis::Postgresql::MetaRasterColumn> &columns) const
{
    QString sqlBuilder;
    if ( !_params.table().isEmpty()) {
        if (!_params.column().isEmpty()) {
            MetaRasterColumn meta;
            meta.catalog = _params.database();
            meta.schema = _params.schema();
            meta.tableName = _params.table();
            meta.rasterColumn = _params.column();
            QString sqlBuilder;
            if (!_params.rasterID().isEmpty())
                sqlBuilder = "SELECT ST_SRID(" + _params.column() + ") FROM " + _params.schema() + "." + _params.table() + " WHERE rid=" + _params.rasterID();
            else
                sqlBuilder = "SELECT ST_SRID(" + _params.column() + ") FROM " + _params.schema() + "." + _params.table() + " WHERE " + _params.column() + " IS NOT NULL LIMIT 1";
            QSqlQuery query = doQuery(sqlBuilder);
            if (query.next()) {
                QString srid = query.value(0).toString();
                ICoordinateSystem crs;
                prepareCoordinateSystem(srid, crs);
                meta.crs = crs;
            }
            columns.push_back(meta);
        } else {
            sqlBuilder = "SELECT column_name FROM INFORMATION_SCHEMA.Columns WHERE table_schema='" + _params.schema() + "' AND table_name='" + _params.table() + "' AND udt_name='raster'";
            QSqlQuery query = doQuery(sqlBuilder);
            while (query.next()) {
                QString columnname = query.value(0).toString();
                MetaRasterColumn meta;
                meta.catalog = _params.database();
                meta.schema = _params.schema();
                meta.tableName = _params.table();
                meta.rasterColumn = columnname;
                sqlBuilder = "SELECT ST_SRID(" + columnname + ") FROM " + _params.schema() + "." + _params.table() + " WHERE " + columnname + " IS NOT NULL LIMIT 1";
                QSqlQuery query2 = doQuery(sqlBuilder);
                if (query2.next()) {
                    QString srid = query2.value(0).toString();
                    ICoordinateSystem crs;
                    prepareCoordinateSystem(srid, crs);
                    meta.crs = crs;
                }
                columns.push_back(meta);
            }
        }
    }
}

void Ilwis::Postgresql::PostgresqlDatabaseUtil::getMetaForGeometryColumns(QList<Ilwis::Postgresql::MetaGeometryColumn> &columns) const
{
    QString sqlBuilder = "SELECT * FROM geometry_columns";
    if ( !_params.schema().isEmpty()) {
        sqlBuilder += " WHERE f_table_schema = '" + _params.schema() + "'";
        if ( !_params.table().isEmpty())
            sqlBuilder += " AND f_table_name = '" + _params.table().toLower() + "'";
    }
    QSqlQuery query = doQuery(sqlBuilder);

    while (query.next()) {
        MetaGeometryColumn meta;
        meta.catalog = query.value("f_table_catalog").toString();
        meta.schema = query.value("f_table_schema").toString();
        meta.tableName = query.value("f_table_name").toString();
        meta.geomColumn = query.value("f_geometry_column").toString();
        meta.dimension = query.value("coord_dimension").toInt();

        ICoordinateSystem crs;
        QString srid = QString::number(query.value("srid").toInt());
        prepareCoordinateSystem(srid, crs);
        meta.crs = crs;

        IlwisTypes ilwisType = itPOINT | itLINE | itPOLYGON;
        QString geomType = query.value("type").toString();
        if (geomType.contains("LINE", Qt::CaseInsensitive) ) {
            ilwisType = itLINE;
        } else if (geomType.contains("POINT", Qt::CaseInsensitive) ) {
            ilwisType = itPOINT;
        } else if (geomType.contains("POLYGON", Qt::CaseInsensitive) ) {
            ilwisType = itPOLYGON;
        }
        meta.geomType = ilwisType;
        columns.push_back(meta);
    }
}

bool PostgresqlDatabaseUtil::createTable(Table *table) const{

    std::vector<QString> dataStatements;
    for(int col=0; col < table->columnCount(); ++col){
        QString name = table->columndefinition(col).name();
        IlwisTypes datatype = table->columndefinition(col).datadef().domain()->ilwisType();
        switch(datatype){
        case itITEMDOMAIN:
        case itTEXTDOMAIN:
            dataStatements.push_back(name + " " + "text");break;
        case itNUMERICDOMAIN:{
            IlwisTypes valueType = table->columndefinition(col).datadef().domain()->valueType();
            if (  hasType(valueType, itINTEGER)){
                dataStatements.push_back(name + " " + "integer");
            }else {
                dataStatements.push_back(name + " " + "real");
            }

        }
            break;
        case itBOOL:
            dataStatements.push_back(name + " " + "boolean");break;
        }
    }
    QString statement = "create table %1.%2(%3)";
    QString columns;
    for(QString column : dataStatements){
        if ( columns != "")
            columns += ",";
        columns += column;
    }
    QString expr = statement.arg(_params.schema()).arg(_params.table().toLower()).arg(columns);
    doQuery(expr);

    return true;
}
bool PostgresqlDatabaseUtil::createTable(FeatureCoverage *fcoverage) const{
    const FeatureAttributeDefinition& attributes =  fcoverage->attributeDefinitions();

    std::vector<QString> dataStatements;
    for(int col=0; col < attributes.columnCount(); ++col){
        QString name = attributes.columndefinition(col).name();
        IlwisTypes datatype = attributes.columndefinition(col).datadef().domain()->ilwisType();
        switch(datatype){
        case itITEMDOMAIN:
        case itTEXTDOMAIN:
            dataStatements.push_back(name + " " + "text");break;
        case itNUMERICDOMAIN:{
            IlwisTypes valueType = attributes.columndefinition(col).datadef().domain()->valueType();
            if (  hasType(valueType, itINTEGER)){
                dataStatements.push_back("\'" +name + " " + "integer");
            }else {
                dataStatements.push_back(name + " " + "real");
            }

        }
            break;
        case itBOOL:
            dataStatements.push_back(name + " " + "boolean");break;
        }
    }
    QString statement = "create table %1.%2(%3)";
    QString columns;
    for(QString column : dataStatements){
        if ( columns != "")
            columns += ",";
        columns += column;
    }
    QString expr = statement.arg(_params.schema()).arg(_params.table().toLower()).arg(columns);
    doQuery(expr);
    QString csyCode = fcoverage->coordinateSystem()->code();
    int epsg = 0;
    if ( csyCode.indexOf("epsg:") == 0){
        epsg = csyCode.mid(5).toInt() ;
    }else {
        if ( fcoverage->coordinateSystem()->ilwisType() == itCONVENTIONALCOORDSYSTEM){
            IConventionalCoordinateSystem projectedCsy = fcoverage->coordinateSystem().as<ConventionalCoordinateSystem>();
            QString prj4Def = projectedCsy->toProj4();
            Proj4Def def = Proj4Parameters::lookupDefintion(prj4Def);
            if ( def._epsg != ""){
                epsg = def._epsg.toInt();
            }
        }
    }
    statement = "Select AddGeometryColumn ('%1','%2', 'geometries',%3,'%4',2)";
    expr = statement.arg(_params.schema()).arg(_params.table().toLower()).arg(epsg).arg("GEOMETRY");
    QSqlQuery result  = doQuery(expr);


    return true;
}

bool PostgresqlDatabaseUtil::tableExists() const{

    QString query = QString("SELECT EXISTS( SELECT * FROM information_schema.tables  where table_schema='%1' and table_name='%2')").arg(_params.schema()).arg(_params.table());
    QSqlQuery result = doQuery(query);
    while(result.next()){
        return result.value(0).toBool();
    }
    return false;
}

bool Ilwis::Postgresql::PostgresqlDatabaseUtil::exists(Ilwis::SPFeatureI feature) const
{
    SqlStatementHelper sqlHelper(_params);

    QString where;
    QList<QString> primaryKeys;
    getPrimaryKeys(primaryKeys);
    Record record = feature->record();
    foreach (QString primaryKey, primaryKeys) {
        if (where.isEmpty()) {
            where.append(" WHERE ");
        } else {
            where.append(" AND ");
        }
        ColumnDefinition coldef = feature->attributedefinition(primaryKey);
        QVariant value = record.cell(coldef.columnindex());

        where.append(primaryKey).append(" = ");
        where.append(sqlHelper.createInsertValueString(value, coldef));
    }

    QString sql = "SELECT ";
    sql.append(QStringList(primaryKeys).join(","));
    sql.append(" FROM ").append(_params.schema() + "." + _params.table());
    sql.append(where);

    QSqlQuery query = doQuery(sql);
    return query.next();
}

void Ilwis::Postgresql::PostgresqlDatabaseUtil::getPrimaryKeys(QList<QString> &primaryColumns) const
{
    if ( primaryColumns.size() == 0)
        return;

    QString qtablename = _params.schema() + "." + _params.table();
    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(" pg_attribute.attname ");
    //sqlBuilder.append(" , format_type(pg_attribute.atttypid, pg_attribute.atttypmod) ");
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(" pg_index, pg_class, pg_attribute ");
    sqlBuilder.append(" WHERE ");
    sqlBuilder.append(" pg_class.oid = '").append(qtablename).append("'").append("::regclass ");
    sqlBuilder.append(" AND ");
    sqlBuilder.append(" indrelid = pg_class.oid ");
    sqlBuilder.append(" AND ");
    sqlBuilder.append("pg_attribute.attrelid = pg_class.oid ");
    sqlBuilder.append(" AND ");
    sqlBuilder.append(" pg_attribute.attnum = any(pg_index.indkey) ");
    sqlBuilder.append(" AND ");
    sqlBuilder.append(" indisprimary"); // ignore indexed only columns
    sqlBuilder.append(" ;");

    QSqlQuery query = doQuery(sqlBuilder);
    while (query.next()) {
        primaryColumns.append(query.value(0).toString());
    }
}

void Ilwis::Postgresql::PostgresqlDatabaseUtil::prepareCoordinateSystem(QString srid, Ilwis::ICoordinateSystem &crs) const
{
    QString code = QString("code=epsg:%1").arg(srid);
    if ( !crs.prepare(code, itCONVENTIONALCOORDSYSTEM)) {
        ERROR1("Could not prepare crs with %1.", code);
    }
}

void Ilwis::Postgresql::PostgresqlDatabaseUtil::prepareSubFeatureSemantics(Ilwis::IDomain &domain, const QList<MetaGeometryColumn> &geomColumns, const IOOptions &options) const
{
    QString columns;
    if ( !options.contains("pg.features.order")) {
        auto join = [&columns](MetaGeometryColumn item) {
            QString column = item.geomColumn;
            return columns.isEmpty()
                    ? columns.append(column)
                    : columns.append(",").append(column);
        };
        std::for_each(geomColumns.begin(), geomColumns.end(), join);
    } else {
        columns = options["pg.features.order"].toString();
    }
    QStringList orderedColumns = columns.split(",");
    NamedIdentifierRange priorities;
    foreach (QString column, orderedColumns) {
        priorities << column.trimmed();         
    }
    IThematicDomain trackIdx;
    trackIdx.prepare();
    trackIdx->setRange(priorities);
    domain = trackIdx;
}

QSqlQuery Ilwis::Postgresql::PostgresqlDatabaseUtil::doQuery(QString stmt) const
{
    QSqlDatabase db = open();
    QSqlQuery query(db);
    bool ok = query.exec(stmt);
    if (!ok || !query.isActive()) {
        QString error = db.lastError().text();
        ERROR2("Could not execute sql statement (error: '%1'): '%2'", error, stmt);
    } else {
        //DEBUG1("SQL: '%1'", stmt);
    }
    return query;
}
