#ifndef WFSConnection_H
#define WFSConnection_H

namespace Ilwis {
namespace Wfs {

class WFSConnection :public OperationImplementation
{

public:

    WFSConnection(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(WFSConnection);

private:
    QString _host;
    QString _username = sUNDEF;
    QString _password = sUNDEF;
    quint32 _port = iUNDEF;
    QString _path = sUNDEF;
    QString _version = sUNDEF;
};
}
}

#endif // WFSConnection_H
