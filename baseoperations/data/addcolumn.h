#ifndef ADDCOLUMN_H
#define ADDCOLUMN_H


namespace Ilwis {
namespace BaseOperations {
class AddColumn :  public OperationImplementation
{
public:
    AddColumn();

    AddColumn(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(AddColumn);

private:
    ITable _inputTable;
    QString _columnName;
    IDomain _columnDomain;
};
}
}

#endif // ADDCOLUMN_H
