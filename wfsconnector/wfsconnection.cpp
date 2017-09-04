#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "wfsconnection.h"

using namespace Ilwis;
using namespace Wfs;

REGISTER_OPERATION(WFSConnection)

WFSConnection::WFSConnection(quint64 metaid, const Ilwis::OperationExpression &expression) : OperationImplementation(metaid, expression)
{

}

bool WFSConnection::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    QString expression = "http://"+ _host;
    if ( _port != 0)
        expression += ":" + QString::number(_port);
    if ( _path != "") {
        expression += "/" + _path;
    }
    expression += QString("/wfs?service=WFS&%1REQUEST=GetCapabilities&ACCEPTVERSIONS=1.1.0");

    if ( _username != "" && _password != ""){
        expression += QString("&username=%1&password=%2").arg(_username).arg(_password);
    }

    QVariant value;
    value.setValue<QString>(expression);
    ctx->setOutput(symTable, value, "connectionstring", itSTRING, Resource());

    return true;
}

Ilwis::OperationImplementation *WFSConnection::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return  new WFSConnection(metaid, expr);
}

Ilwis::OperationImplementation::State WFSConnection::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    _host = _expression.input<QString>(0);
    _port = _expression.input<quint32>(1);
    _path = _expression.input<QString>(2);
    //_version = _expression.input<QString>(3);
    _username = _expression.input<QString>(3);
    _password = _expression.input<QString>(4);
    if ( _host == "")
        return sPREPAREFAILED;

    return sPREPARED;
}

quint64 WFSConnection::createMetadata()
{
    OperationResource operation({"ilwis://operations/wfscatalog"});
    operation.setLongName("Web Feature Service Catalog");
    operation.setSyntax("wfscatalog(host[,port],path[,username, password])");
    operation.setDescription(TR("creates a url to access the catalog of a wfs server"));
    operation.setInParameterCount({3,4,5});
    operation.addInParameter(0,itSTRING , TR("host address"),TR("identifies the host that is running the wfs server"));
    operation.addInParameter(1,itPOSITIVEINTEGER , TR("port number"),TR("port used on the remote server"));
    operation.addInParameter(2,itSTRING , TR("path/server"),TR("Additional path on the wfs server(usualy a server type)"));
    // only 1.1.0 is supported and added automatically
    //operation.addInParameter(3,itSTRING , TR("version"),TR("Additional path on the wfs server(usualy a server type)"));
    operation.addInParameter(3,itSTRING , TR("username"),TR("username for authentication on the remote server"));
    operation.addInParameter(4,itSTRING , TR("password"),TR("password for authentication on the remote server"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itSTRING, TR("connection url"),TR("string that is sufficient to access the remote catalog"));
    operation.setKeywords("service,wfs");

    mastercatalog()->addItems({operation});
    return operation.id();
}
