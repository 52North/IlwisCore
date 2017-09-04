#ifndef REQUESTHANDLERFACTORY_H
#define REQUESTHANDLERFACTORY_H

#include <functional>
#include "httpserver_global.h"

typedef std::function<HttpRequestHandler*()> CreateHandler;

namespace Ilwis {
namespace HTTP {


class HTTPSERVER_EXPORT HTTPRequestHandlerFactory : public AbstractFactory
{
public:
    HTTPRequestHandlerFactory();
    HttpRequestHandler *create(const QString &type);

    void addHandler(const QString &type, CreateHandler handler);
private:
    std::map<QString, CreateHandler> _createMethods;
};
}
}

#endif // REQUESTHANDLERFACTORY_H
