
#include <QSqlError>
#include <QSqlDatabase>

#include "kernel.h"
#include "ilwisdata.h"
#include "ilwistypes.h"
#include "resource.h"
#include "domain.h"
#include "geometries.h"
#include "numericdomain.h"
#include "textdomain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "attributedefinition.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "rastercoverage.h"
#include "raster.h"
#include "feature.h"
#include "table.h"

#include "postgresqltableloader.h"
#include "postgresqldatabaseutil.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlTableLoader::PostgresqlTableLoader(const Resource &resource, const IOOptions &options): _resource(resource), _options(options)
{
}

PostgresqlTableLoader::~PostgresqlTableLoader()
{
}

bool PostgresqlTableLoader::loadMetadata(Table *table) const
{
    PostgresqlParameters params (_resource.url(true).toString());
    PostgresqlDatabaseUtil pgUtil(params);

    QString rawTablename(params.schema() + "." + params.table());
    QStringList tableNameList = rawTablename.split(".", QString::SkipEmptyParts);
    QString tableName = tableNameList.at(1);
    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(" column_name,udt_name ");
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(" information_schema.columns ");
    sqlBuilder.append(" WHERE ");
    sqlBuilder.append(" table_name='").append(tableName).append("';");
    QSqlQuery columnTypesQuery = pgUtil.doQuery(sqlBuilder);
    QList<QString> primaryKeys;
    pgUtil.getPrimaryKeys(primaryKeys);

    while (columnTypesQuery.next()) {
        QString columnName = columnTypesQuery.value(0).toString();
        if ( !createColumnDefinition(table, columnTypesQuery, primaryKeys)) {
            if ( !columnTypesQuery.isValid()) {
                WARN("no data record selected.");
            } else {
                DEBUG2("Ignore column '%1' in table '%2'", columnName, tableName);
            }
        }
    }
    return table->isValid();
}

QString PostgresqlTableLoader::select(QString columns) const
{
    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(columns);
    sqlBuilder.append(" FROM ");
    PostgresqlParameters params (_resource.url(true).toString());
    sqlBuilder.append(params.schema() + "." + params.table());
    return sqlBuilder;
}

bool PostgresqlTableLoader::loadData(Table *table) const
{
    QString allNonGeometryColumns;
    for (int i = 0; i < table->columnCount(); i++) {
        ColumnDefinition& coldef = table->columndefinitionRef(i);
        allNonGeometryColumns.append(" ").append(coldef.name()).append(" ,");
    }
    allNonGeometryColumns = allNonGeometryColumns.left(allNonGeometryColumns.length() - 1);

    PostgresqlParameters params (_resource.url(true).toString());
    PostgresqlDatabaseUtil pgUtil(params);
    QSqlQuery query = pgUtil.doQuery(select(allNonGeometryColumns));

    quint64 count = 0;
    table->dataLoaded(true); // prevent any succesfull trying to load the table while its already loading
    std::vector<QVariant> record(table->columnCount());
    std::vector<quint32> columnMap;

    while (query.next()) {
        for (int i = 0; i < table->columnCount(); i++) {
            DataDefinition& datadef = table->columndefinitionRef(i).datadef();
            if( !datadef.domain().isValid()) {
                ColumnDefinition& coldef = table->columndefinitionRef(i);
                WARN2(ERR_NO_INITIALIZED_2, "domain", coldef.name());
                record[i] = QVariant(); // empty
                continue;
            }

            if ( columnMap.size() == 0){
                const QSqlRecord& sqlrecord = query.record();
                columnMap.resize( table->columnCount());
                for (int col = 0; col < table->columnCount(); col++) {
                    ColumnDefinition& coldef = table->columndefinitionRef(col);
                    int index = sqlrecord.indexOf(coldef.name());
                    columnMap[col] = index;
                }
            }

            record[i] = query.value(columnMap[i]);
        }
        table->record(count++, record);
    }
    return true;
}

bool PostgresqlTableLoader::createColumnDefinition(Table *table, const QSqlQuery &query, QList<QString> &primaryKeys) const
{
    if ( !query.isValid()) {
        WARN("Can't create columndefinition for an invalid data record.");
        return false;
    }

    QString columnName = query.value(0).toString();
    QString udtName = query.value(1).toString();

    IDomain domain;
    if (udtName == "varchar" || udtName == "text" || udtName == "char") {
        domain.prepare("code=domain:text",itTEXTDOMAIN);
    } else if (udtName.startsWith("timestamp")) {
        // TODO timestamp vs. timestamptz?
        ITimeDomain tdomain;
        tdomain.prepare();
        tdomain->range(new TimeInterval(itDATE));
        domain = tdomain;
    } else if (udtName.startsWith("float") || udtName == "numeric") {
        INumericDomain ndomain;
        ndomain.prepare("value");
        domain = ndomain;
    } else if (udtName.startsWith("int")) {
        INumericDomain ndomain;
        ndomain.prepare("integer");
        domain = ndomain;
    } else if (udtName == "bool") {
        domain.prepare("boolean", itBOOL);
    } else if (udtName == "geometry") {
        return true; // handled automatically
    } else {
        MESSAGE1("No domain handle for db type '%1')", udtName);
        return false;
    }

    bool readonly = primaryKeys.contains(columnName);
    table->addColumn(columnName, domain, readonly);
    return true;
}
