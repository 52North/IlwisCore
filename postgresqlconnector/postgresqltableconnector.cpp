
#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "juliantime.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"

#include "sqlstatementhelper.h"
#include "postgresqldatabaseutil.h"
#include "postgresqlconnector.h"
#include "postgresqltableconnector.h"
#include "postgresqltableloader.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlTableConnector::PostgresqlTableConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : PostgresqlConnector(resource, load,options)
{
}

IlwisObject *PostgresqlTableConnector::create() const
{
    return new FlatTable(source());
}

ConnectorInterface *PostgresqlTableConnector::create(const Ilwis::Resource &resource, bool load,const IOOptions& options)
{
    return new PostgresqlTableConnector(resource, load, options);
}

bool PostgresqlTableConnector::loadMetaData(IlwisObject *data, const IOOptions& options)
{
    Table *table = static_cast<Table *>(data);
    IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    PostgresqlTableLoader loader = PostgresqlTableLoader(source(), iooptions);
    return loader.loadMetadata(table);
}

bool PostgresqlTableConnector::store(IlwisObject *data, const IOOptions& options)
{
    Table *table = static_cast<Table *>(data);
    IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    PostgresqlParameters params (_resource.url(true).toString());
    PostgresqlDatabaseUtil pgUtil(params);
    SqlStatementHelper sqlHelper(params);

    bool newTable = false;
    if ( !pgUtil.tableExists()){
        if(!pgUtil.createTable(table)){
            return false;
        }
        newTable = true;
    }

    QList<QString> primaryKeys;
    pgUtil.getPrimaryKeys(primaryKeys);
    sqlHelper.addCreateTempTableStmt("data_level_0");
    sqlHelper.addInsertChangedDataToTempTableStmt("data_level_0", table);
    if (!newTable)
        sqlHelper.addUpdateStmt(primaryKeys, "data_level_0", table);
    sqlHelper.addInsertStmt(primaryKeys, "data_level_0", table);

    // TODO delete deleted rows

    //qDebug() << "SQL: " << sqlHelper->sql();

    pgUtil.doQuery(sqlHelper.sql());

    return true;
}

QString PostgresqlTableConnector::trimAndRemoveLastCharacter(const QString &string) {
    return string.left(string.trimmed().length() - 1);
}

QString PostgresqlTableConnector::createInsertValueString(QVariant value, const ColumnDefinition &coldef) const {
    IDomain domain = coldef.datadef().domain<>();
    if (hasType(domain->valueType(),itINTEGER)) {

        return QString::number(value.toInt());

    } else if (hasType(domain->valueType(),itDOUBLE | itFLOAT)) {

        return QString::number(value.toDouble());

    } else if (hasType(domain->valueType(),itTHEMATICITEM | itNAMEDITEM | itINDEXEDITEM | itNUMERICITEM | itTIMEITEM)) {

        return domain->impliedValue(value).toString();

    } else if (hasType(domain->valueType(), itDATETIME)) {
        if ( QString(value.typeName()).compare("Ilwis::Time") != 0){
            ERROR2(ERR_COULD_NOT_CONVERT_2,value.toString(), "time");
            Time time(0,0,0,0,0);
            return time.toString(itDATE);
        } else{
            Time time = value.value<Ilwis::Time>();
            return time.toString(itDATETIME);
        }
    } else if (hasType(domain->valueType(),itSTRING)){

        return QString("'%1'").arg(value.toString());

    } else {
        ERROR0("Could not determine data type.");
        return QString("");
    }
}

bool PostgresqlTableConnector::loadData(IlwisObject *data,const IOOptions& options)
{
    Table *table = static_cast<Table *>(data);
    IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    PostgresqlTableLoader loader = PostgresqlTableLoader(source(), iooptions);
    if ( !loader.loadMetadata(table)) {
        return false;
    }
    bool ok = loader.loadData(table);
    _binaryIsLoaded = ok;
    return ok;
}

