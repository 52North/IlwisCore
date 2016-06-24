#ifndef UNARYMATHRASTERANDNUMBER_H
#define UNARYMATHRASTERANDNUMBER_H


namespace Ilwis {
class NumericRange;

namespace BaseOperations{

typedef std::function<double(double)> UnaryFunction;

class UnaryMathRasterAndNumber : public UnaryMath
{
public:
    UnaryMathRasterAndNumber();
    UnaryMathRasterAndNumber(quint64 metaid, const Ilwis::OperationExpression &expr, const QString &outputdom, UnaryFunction fun);

protected:
    static Resource populateMetadata(const QString &item, const QString &longnamem);
    Ilwis::OperationImplementation::State prepare(Ilwis::ExecutionContext *, const Ilwis::SymbolTable &);
    bool execute( ExecutionContext *ctx, SymbolTable& symTable);
    virtual NumericRange *constructRangeFrom(const SPNumericRange &range);

    IRasterCoverage _inputGC;
    IRasterCoverage _outputGC;
    BoundingBox _box;
    OperationType _case;
    double _number;

};
}
}

#endif // UNARYMATHRASTERANDNUMBER_H
