#ifndef TABLEVALUE_H
#define TABLEVALUE_H


namespace Ilwis {
namespace BaseOperations {
class TableValue : public OperationImplementation
{
public:
    TableValue();
    TableValue(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &);

    static quint64 createMetadata();
private:
    ITable _inputTable;
    QString _columnName;
    int _recordNr;

    NEW_OPERATION(TableValue);

};
}
}
#endif // TABLEVALUE_H
