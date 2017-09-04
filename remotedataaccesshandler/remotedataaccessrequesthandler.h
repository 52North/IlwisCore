#ifndef RemoteDataAccessRequestHandler_H
#define RemoteDataAccessRequestHandler_H

#include "kernel.h"
#include "ilwisdata.h"
#include "catalog.h"
#include "httpserver/httpserver/httprequest.h"
#include "httpserver/httpserver/httpresponse.h"
#include "httpserver/httpserver/httprequesthandler.h"

class QSqlQuery;
class QSqlDatabase;
class QBuffer;

namespace Ilwis {
namespace RemoteDataAccess {


class RemoteDataAccessRequestHandler : public HttpRequestHandler
{
    Q_OBJECT
public:
    RemoteDataAccessRequestHandler();

    void service(HttpRequest& request, HttpResponse& response);
    static HttpRequestHandler *create();

public slots:
    void sendData(QBuffer *buf, bool lastBlock);
private:
    ICatalog _datafolder;
    ICatalog _internalCatalog;
    HttpResponse *_response;

    IIlwisObject getObject(const QString &name, const QString &ilwTypeName);
    void writeObject(const IIlwisObject &obj, const HttpRequest &request, HttpResponse &response);

    //void writeObject(const IIlwisObject &obj, const QString &typeName, HttpResponse &response);
};
}
}

#endif // RemoteDataAccessRequestHandler_H
