#ifndef CREATETABLE_H
#define CREATETABLE_H


namespace Ilwis {
namespace BaseOperations {
class CreateTable :  public OperationImplementation
{
public:
    CreateTable();

    CreateTable(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(CreateTable);

private:
    QString _outputName;
};
}
}

#endif // CREATETABLE_H
