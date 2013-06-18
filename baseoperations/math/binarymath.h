#ifndef PLUS_H
#define PLUS_H

namespace Ilwis {
namespace BaseOperations{
class BinaryMath : public OperationImplementation
{
public:
    enum OperatorType{ otPLUS, otMINUS, otMULT, otDIV};
    BinaryMath();
    BinaryMath(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    State prepare(ExecutionContext *ctx=0);
private:
    bool executeCoverageNumber(Ilwis::ExecutionContext *ctx, SymbolTable& symTable);
    bool executeCoverageCoverage(Ilwis::ExecutionContext *ctx, SymbolTable& symTable);
    bool prepareCoverageCoverage();
    bool prepareCoverageNumber(IlwisTypes ptype1, IlwisTypes ptype2);
    bool setOutput(ExecutionContext *ctx, SymbolTable& symTable);

    bool _coveragecoverage;
    IGridCoverage _inputGC1;
    IGridCoverage _inputGC2;
    IGridCoverage _outputGC;
    double _number;
    Box3D<qint32> _box;
    OperatorType _operator;
};
}
}

#endif // PLUS_H
