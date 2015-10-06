#ifndef AGGREGATION_H
#define AGGREGATION_H
namespace Ilwis {
namespace BaseOperations{
class Aggregation : public NumericOperation
{
public:
    Aggregation();
    Aggregation(quint64 metaid, const Ilwis::OperationExpression &expr);
    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    State prepare(ExecutionContext *ctx,const SymbolTable&);

private:
    ITable _inputTable;
    ITable _outputTable;
    QString _column1 = sUNDEF;
    QString _column2 = sUNDEF;
    QString _outColumn1 = sUNDEF;
    QString _outColumn2 = sUNDEF;
    NEW_OPERATION(Aggregation)    ;
};
}
}


#endif // AGGREGATION_H
