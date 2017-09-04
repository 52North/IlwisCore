/**
  @file
  @author Stefan Frings
*/
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"
#include <memory>
#include "httprequesthandler.h"
#include "../requesthandlerfactory.h"
#include "requestmapper.h"

using namespace Ilwis;

RequestMapper::RequestMapper(QObject* parent)
    :HttpRequestHandler(parent) {

}

RequestMapper::~RequestMapper(){
}

void RequestMapper::service(HttpRequest& request, HttpResponse& response) {
    QByteArray path=request.getPath();
    qDebug("RequestMapper: path=%s",path.data());

    UPHTTPRequestHandler handler;
    HTTP::HTTPRequestHandlerFactory *factory = kernel()->factory<HTTP::HTTPRequestHandlerFactory>("ilwis::HTTPRequestHandlerFactory");
    handler.reset(factory->create(path));
    if ( handler)
        handler->service(request, response);
    else{
       handler.reset(factory->create("staticfilecontroller"));
       if ( handler)
           handler->service(request, response);
    }


    qDebug("RequestMapper: finished request");
}
