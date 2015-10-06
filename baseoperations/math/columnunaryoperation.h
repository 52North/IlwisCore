#ifndef COLUMNUNARYOPERATION_H
#define COLUMNUNARYOPERATION_H

namespace Ilwis {
namespace BaseOperations{

class ColumnUnaryOperation : public NumericOperation
{
public:
    ColumnUnaryOperation();
    ColumnUnaryOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    State prepare(ExecutionContext *ctx,const SymbolTable&);
private:
    ITable _inputTable;
    ITable _outputTable;
    QString _column1 = sUNDEF;
    QString _column2 = sUNDEF;
    QString _outColumn = sUNDEF;
    QString _inColumn = sUNDEF;


    NumericStatistics::PropertySets _method;
    bool _grouped;
    std::vector<quint32> _groupSize = {1,1,1};

    NumericStatistics::PropertySets toMethod(const QString &nm);

    NEW_OPERATION(ColumnUnaryOperation);

};
}
}

#endif // COLUMNUNARYOPERATION_H
