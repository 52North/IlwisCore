#ifndef CATALOGCONNECTION_H
#define CATALOGCONNECTION_H

namespace Ilwis {
namespace Stream {

class CatalogConnection :public OperationImplementation
{

public:

    CatalogConnection(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(CatalogConnection);

private:
    QString _host;
    QString _username;
    QString _password;
    quint32 _port;
};
}
}

#endif // CATALOGCONNECTION_H
