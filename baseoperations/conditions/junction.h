#ifndef NUMBERCONDITION_H
#define NUMBERCONDITION_H

namespace Ilwis {
namespace BaseOperations{

class Junction : public OperationImplementation
{
public:
    Junction();
    Junction(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(Junction);
private:
    QString _firstValue, _secondValue, _type;

};
}
}

#endif // NUMBERCONDITION_H
