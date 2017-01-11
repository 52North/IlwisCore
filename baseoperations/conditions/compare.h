#ifndef COMPARE_H
#define COMPARE_H


namespace Ilwis {
namespace BaseOperations{
class Compare : public OperationImplementation
{
public:
    Compare();
    Compare(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(Compare);
private:
    QString _firstValue, _secondValue;
    LogicalOperator _operator;
    bool _numeric;

};
}
}

#endif // COMPARE_H
