#ifndef NUMBERCONDITION_H
#define NUMBERCONDITION_H

namespace Ilwis {
namespace BaseOperations{
class NumberCondition : public NumericOperation
{
public:
    NumberCondition();
    NumberCondition(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(NumberCondition);
private:
    double _firstValue, _secondValue;
    int _condition;

};
}
}

#endif // NUMBERCONDITION_H
