#ifndef REMOTEOPERATIONHANDLER_H
#define REMOTEOPERATIONHANDLER_H
#include "kernel.h"
#include "ilwisdata.h"
#include "catalog.h"
#include "httpserver/httpserver/httprequest.h"
#include "httpserver/httpserver/httpresponse.h"
#include "httpserver/httpserver/httprequesthandler.h"

namespace Ilwis {
namespace RemoteDataAccess {


class RemoteOperationRequestHandler : public HttpRequestHandler
{
public:
    RemoteOperationRequestHandler();

    void service(HttpRequest& request, HttpResponse& response);
    static HttpRequestHandler *create();
};
}
}

#endif // REMOTEOPERATIONHANDLER_H
