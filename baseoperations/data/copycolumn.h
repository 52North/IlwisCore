#ifndef COPYCOLUMN_H
#define COPYCOLUMN_H


namespace Ilwis {
namespace BaseOperations {
class CopyColumn :  public OperationImplementation
{
public:
    CopyColumn();

    CopyColumn(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(CopyColumn);

private:
    ITable _inputTable;
    ITable _outputTable;
    QString _inputColumnName;
    QString _outputColumnName;
    QString _inputKey;
    QString _outputKey;
};
}
}

#endif // COPYCOLUMN_H
