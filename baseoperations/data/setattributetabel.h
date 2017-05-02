#ifndef SETATTRIBUTETABEL_H
#define SETATTRIBUTETABEL_H


namespace Ilwis {
namespace BaseOperations {
class SetAttributeTable : public  OperationImplementation
{
public:
    SetAttributeTable();
    SetAttributeTable(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

private:
    ITable _inputTable;
    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;

    QString _primaryKey;

    NEW_OPERATION(SetAttributeTable);

};
}
}
#endif // SETATTRIBUTETABEL_H
