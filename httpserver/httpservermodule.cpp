#include <QtPlugin>
#include <QSqlQuery>
#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "symboltable.h"
#include "httpservermodule.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "httpserver.h"
#include "httpserver/httprequesthandler.h"
#include "requesthandlerfactory.h"
#include "httpserver/staticfilecontroller.h"


using namespace Ilwis;
using namespace HTTP;

HttpServerModule::HttpServerModule(QObject *parent) :
    Module(parent, "HttpServerModule", "iv40","1.0")
{
}

QString HttpServerModule::getInterfaceVersion() const
{
    return "iv40";

}

void HttpServerModule::prepare()
{
    HTTPRequestHandlerFactory *handlerFactory = new HTTPRequestHandlerFactory();
    kernel()->addFactory(handlerFactory);

    handlerFactory->addHandler("staticfilecontroller",StaticFileController::create);

    kernel()->issues()->log("Loaded module HTTP server",IssueObject::itMessage);


}

QString HttpServerModule::getName() const
{
    return "HTTP Server plugin";
}

QString HttpServerModule::getVersion() const
{
    return "1.0";
}




