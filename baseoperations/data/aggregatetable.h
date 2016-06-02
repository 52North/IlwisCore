#ifndef AGGREGATETABLE_H
#define AGGREGATETABLE_H


namespace Ilwis {
namespace BaseOperations {
class AggregateTable : public  OperationImplementation
{
public:
    AggregateTable();
    AggregateTable(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

private:
    ITable _inputTable;
    int _aggregateColumn;
    ITable _outputTable;
    QString _aggregationMehtod;

    NEW_OPERATION(AggregateTable);
    QVariant numericCase(const std::vector<int>& records, int c);
    QVariant stringCase(IlwisTypes tp, const std::vector<int> &records, int c);
};
}
}

#endif // AGGREGATETABLE_H
