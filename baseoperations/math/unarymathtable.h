#ifndef UNARYMATHTABLE_H
#define UNARYMATHTABLE_H


namespace Ilwis {
namespace BaseOperations{

class UnaryMathTable : public UnaryMath
{
public:
    UnaryMathTable();
    UnaryMathTable(quint64 metaid, const Ilwis::OperationExpression &expr, const QString& outdom,UnaryFunction fun);

protected:
    static Resource populateMetadata(const QString &item, const QString &longname);
    Ilwis::OperationImplementation::State prepare(Ilwis::ExecutionContext *, const Ilwis::SymbolTable &);
    bool execute( ExecutionContext *ctx, SymbolTable& symTable);

    ITable _inputTable;
    ITable _outputTable;
    QString _inColumn;
    QString _outColumn;

};
}
}

#endif // UNARYMATHTABLE_H
