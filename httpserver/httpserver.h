#ifndef HTTPSERVER_H
#define HTTPSERVER_H

class ServerStarter;


namespace Ilwis {
namespace HTTP {
class HTTPServer : public OperationImplementation
{
public:
    HTTPServer();
    HTTPServer(quint64 metaid, const Ilwis::OperationExpression &expr);
    ~HTTPServer() ;
    static quint64 createMetadata();
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable& symTable);
    bool execute(ExecutionContext *ctx, SymbolTable& symTable);

    NEW_OPERATION(HTTPServer);

private:
    static std::unique_ptr<ServerStarter>_server;
    quint16 _port;
};
}
}
#endif // HTTPSERVER_H
