#ifndef SELECTIONTABLE_H
#define SELECTIONTABLE_H

namespace Ilwis {
namespace BaseOperations {
class SelectionTable : public OperationImplementation
{
public:
    SelectionTable();

    SelectionTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    ~SelectionTable();

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);

    static quint64 createMetadata();
private:
    ITable _inputTable;
    ITable _outputTable;
};
}
}

#endif // SELECTIONTABLE_H
