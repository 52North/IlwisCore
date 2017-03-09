#ifndef BINARYMATHRASTER_H
#define BINARYMATHRASTER_H

namespace Ilwis {
namespace BaseOperations{
class BinaryMathRaster : public NumericOperation
{
public:
    BinaryMathRaster();
    BinaryMathRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    State prepare(ExecutionContext *ctx,const SymbolTable&);
private:
    bool executeCoverageNumber(Ilwis::ExecutionContext *ctx, SymbolTable& symTable);
    bool executeCoverageCoverage(Ilwis::ExecutionContext *ctx, SymbolTable& symTable);
    bool prepareCoverageCoverage();
    bool prepareCoverageNumber(IlwisTypes ptype1, IlwisTypes ptype2);
    bool setOutput(ExecutionContext *ctx, SymbolTable& symTable);

    bool _coveragecoverage;
    IRasterCoverage _inputGC1;
    IRasterCoverage _inputGC2;
    IRasterCoverage _outputGC;
    double _leftValue = rUNDEF;
    double  _rightValue = rUNDEF;
    BoundingBox _box;

    NEW_OPERATION(BinaryMathRaster)    ;
};
}
}

#endif // BINARYMATHRASTER_H
