#ifndef BINARYMATHTABLE_H
#define BINARYMATHTABLE_H

namespace Ilwis {
namespace BaseOperations{


class BinaryMathTable : public NumericOperation
{
public:
    BinaryMathTable();
    BinaryMathTable(quint64 metaid, const Ilwis::OperationExpression &expr);

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

    NEW_OPERATION(BinaryMathTable)    ;

};
}
}

#endif // BINARYMATHTABLE_H
