#include "kernel.h"
#include "ilwisdata.h"
#include "resource.h"
#include "identity.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "httpserver/httplistener.h"
#include "httpserver.h"
#include "httpserver/requestmapper.h"
#include "httpserver/serverstarter.h"

using namespace Ilwis;
using namespace HTTP;

std::unique_ptr<ServerStarter> HTTPServer::_server;

REGISTER_OPERATION(HTTPServer)

OperationImplementation *HTTPServer::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new HTTPServer(metaid, expr);
}

HTTPServer::HTTPServer()
{
}

HTTPServer::HTTPServer(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
    _server.reset(new ServerStarter);
}

HTTPServer::~HTTPServer()
{

}

OperationImplementation::State HTTPServer::prepare(ExecutionContext *ctx, const SymbolTable& symTable) {

    bool ok;
    int port = _expression.parm(0).value().toLong(&ok);
    if ( !ok || port < 1024 || port > 49151){
        ERROR2(ERR_INVALID_PROPERTY_FOR_2, TR("port number"), "http server");
    }
    _port = port;
    return OperationImplementation::sPREPARED;
}

bool HTTPServer::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    std::map<QString, QVariant> arguments;
    arguments["port"] = _port;
    if ( _server)
        _server->exec(arguments);

    return true;
}

quint64 HTTPServer::createMetadata()
{
    OperationResource operation({"ilwis://operations/httpserver"});
    operation.setSyntax("httpserver([port]");
    operation.setDescription(TR("transpose the raster according to the method indicated by the second parameter"));
    operation.setInParameterCount({0,1});
    operation.addOptionalInParameter(0,itUINT16,  TR("port number"));

    operation.setOutParameterCount({0});
    operation.setKeywords("server, http");

    mastercatalog()->addItems({operation});
    return operation.id();
}


