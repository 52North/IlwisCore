#ifndef TABLEVALUEBYPRIMARYKEY_H
#define TABLEVALUEBYPRIMARYKEY_H


namespace Ilwis {
namespace BaseOperations {
class TableValueByPrimaryKey : public OperationImplementation
{
public:
    TableValueByPrimaryKey();
    TableValueByPrimaryKey(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &);

    static quint64 createMetadata();
private:
    ITable _inputTable;
    QString _primaryColumnName;
    QString _primaryKeyValue;
    QString _valueColumn;

    NEW_OPERATION(TableValueByPrimaryKey);

};
}
}

#endif // TABLEVALUEBYPRIMARYKEY_H
