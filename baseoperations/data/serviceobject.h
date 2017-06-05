#ifndef ASPECT_H
#define ASPECT_H


namespace Ilwis {
namespace BaseOperations{
class ServiceObject : public OperationImplementation
{
public:
    ServiceObject();
    ServiceObject(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(ServiceObject);
private:
    IIlwisObject _object;
    QString _objectType;


    void coordinateSystem(const IIlwisObject &obj, ExecutionContext *ctx, SymbolTable &symTable);
};
}
}

#endif // ASPECT_H
