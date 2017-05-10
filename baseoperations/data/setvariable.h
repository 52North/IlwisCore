#ifndef SETVARIABLE_H
#define SETVARIABLE_H

namespace Ilwis {
namespace BaseOperations {
class SetVariable : public OperationImplementation
{
public:
    SetVariable();
    SetVariable(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &);

    static quint64 createMetadata();
private:
    QString _value;

    NEW_OPERATION(SetVariable);

};
}
}

#endif // SETVARIABLE_H
