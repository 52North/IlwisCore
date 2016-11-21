#ifndef RASTERSLICING_H
#define RASTERSLICING_H

namespace Ilwis {
namespace RasterOperations {
class RasterSlicing : public OperationImplementation
{
public:
    RasterSlicing();

    RasterSlicing(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    IIntervalDomain _numericItems;
    std::vector<Interval *> _bounds;

    NEW_OPERATION(RasterSlicing);

};
}
}

#endif // RASTERSLICING_H
