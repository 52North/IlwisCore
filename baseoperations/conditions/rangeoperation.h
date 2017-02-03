#ifndef RANGE1_H
#define RANGE1_H


namespace Ilwis {
namespace BaseOperations{

class RangeOperation : public OperationImplementation
{
public:
    RangeOperation();
    RangeOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(RangeOperation);
private:


};
}
}
#endif // RANGE1_H
