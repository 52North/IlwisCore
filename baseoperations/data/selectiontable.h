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
    std::vector<QString> _columns;
    std::vector<std::pair<quint32, quint32>> _records;

    NEW_OPERATION(SelectionTable);
};
}
}

#endif // SELECTIONTABLE_H
