#ifndef CONNECT_H
#define CONNECT_H


namespace Ilwis {
namespace BaseOperations {
class Connect : public OperationImplementation
{
public:
    Connect();
    Connect(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &);

    static quint64 createMetadata();
private:
    IIlwisObject _object;

    NEW_OPERATION(Connect);

};
}
}

#endif // CONNECT_H
