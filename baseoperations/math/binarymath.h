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

    State prepare(ExecutionContext *ctx,const SymbolTable&);
private:
    bool executeRasterNumber(Ilwis::ExecutionContext *ctx, SymbolTable& symTable);
    bool executeRasterRaster(Ilwis::ExecutionContext *ctx, SymbolTable& symTable);
    bool prepareRasterRaster();
    bool prepareRasterNumber(IlwisTypes ptype1, IlwisTypes ptype2);
    bool setOutput(ExecutionContext *ctx, SymbolTable& symTable);

    bool _coveragecoverage;
    IRasterCoverage _inputGC1;
    IRasterCoverage _inputGC2;
    IRasterCoverage _outputGC;
    double _number;
    Box3D<qint32> _box;
    OperatorType _operator;
};
}
}

#endif // PLUS_H
