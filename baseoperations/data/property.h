#ifndef PROPERTY_H
#define PROPERTY_H


namespace Ilwis {
namespace BaseOperations {
class Property : public OperationImplementation
{
public:
    Property();
    Property(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &);

    static quint64 createMetadata();
private:
    IIlwisObject _object;
    QString _property;

    NEW_OPERATION(Property);

};
}
}

#endif // PROPERTY_H
