#ifndef ISCOMPATIBLE_H
#define ISCOMPATIBLE_H


namespace Ilwis {
namespace BaseOperations{
class IsCompatible : public OperationImplementation
{
public:
    IsCompatible();
    IsCompatible(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(IsCompatible);
private:
    IIlwisObject _object1, _object2;
    IlwisTypes _role;


};
}
}

#endif // ISCOMPATIBLE_H
