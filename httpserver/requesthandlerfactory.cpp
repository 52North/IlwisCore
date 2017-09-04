#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"
#include <memory>
#include "httpserver/httprequesthandler.h"
#include "requesthandlerfactory.h"

using namespace Ilwis;
using namespace HTTP;

HTTPRequestHandlerFactory::HTTPRequestHandlerFactory() : AbstractFactory("HTTPRequestHandlerFactory","ilwis")
{
}


HttpRequestHandler *HTTPRequestHandlerFactory::create(const QString& type) {
    auto iter = _createMethods.find(type);
    if ( iter != _createMethods.end())
        return iter->second();
    return 0;
}

void HTTPRequestHandlerFactory::addHandler(const QString& type, CreateHandler handler){
    _createMethods[type] = handler;
}

