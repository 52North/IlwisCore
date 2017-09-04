#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "catalog.h"
#include "mastercatalog.h"
#include <iostream>
#include "postgresqlconnection.h"

using namespace Ilwis;
using namespace Postgresql;

REGISTER_OPERATION(PostgresqlConnection)

PostgresqlConnection::PostgresqlConnection(quint64 metaid, const Ilwis::OperationExpression &expression) : OperationImplementation(metaid, expression)
{

}

bool PostgresqlConnection::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    QString expression = _params.toString();
    QVariant value;
    value.setValue<QString>(expression);
    ctx->setOutput(symTable, value, "connectionstring", itSTRING, Resource());

    return true;
}

Ilwis::OperationImplementation *PostgresqlConnection::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return  new PostgresqlConnection(metaid, expr);
}

Ilwis::OperationImplementation::State PostgresqlConnection::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    if (_expression.parameterCount() > 3) {
        QString user = _expression.input<QString>(0);
        QString pass = _expression.input<QString>(1);
        QString host = _expression.input<QString>(2);
        quint32 port = _expression.input<quint32>(3);
        QString database = "";
        QString schema = "";
        QString table = "";
        QString column = "";
        if (_expression.parameterCount() > 4) {
            database = _expression.input<QString>(4);
            if (_expression.parameterCount() > 5) {
                schema = _expression.input<QString>(5);
                if (_expression.parameterCount() > 6) {
                    table = _expression.input<QString>(6);
                    if (_expression.parameterCount() > 7) {
                        column = _expression.input<QString>(7);
                    }
                }
            }
        }
        _params = PostgresqlParameters(user, pass, host, port, database, schema, table, column);
        return sPREPARED;
    } else
        return sPREPAREFAILED;
}

quint64 PostgresqlConnection::createMetadata()
{
    OperationResource operation({"ilwis://operations/postgresqlcatalog"});
    operation.setLongName("PostgreSQL Catalog");
    operation.setSyntax("postgresqlcatalog(username,password,host,port,database,schema,table,column,rasterid)");
    operation.setDescription(TR("creates a url to access the catalog of a postgresql database; the url must have look like postgresql://mysusername:mypassword@somehost:someport/adataabasename"));
    operation.setInParameterCount({9});
    operation.addInParameter(0, itSTRING, TR("username"),TR("username for authentication on the remote server"));
    operation.addInParameter(1, itSTRING, TR("password"),TR("password for authentication on the remote server"));
    operation.addInParameter(2, itSTRING, TR("host address"),TR("identifies the host that is running the posgresql database"));
    operation.addInParameter(3, itPOSITIVEINTEGER, TR("port number"),TR("port used on the remote server"));
    operation.addOptionalInParameter(4,itSTRING , TR("database"),TR("The database name on the server"));
    operation.addOptionalInParameter(5,itSTRING , TR("schema"),TR("The database schema in the database"));
    operation.addOptionalInParameter(6,itSTRING , TR("table"),TR("The table in the specified schema"));
    operation.addOptionalInParameter(7,itSTRING , TR("column"),TR("The geometry or raster column in the table"));
    operation.addOptionalInParameter(8,itSTRING , TR("rasterid"),TR("The rasterid of a multiraster column in the table"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itSTRING, TR("connection url"),TR("string that is sufficient to access the remote catalog"));
    operation.setKeywords("service,postgresql,database");
    mastercatalog()->addItems({operation});
    return operation.id();
}
