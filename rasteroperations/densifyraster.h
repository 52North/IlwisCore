#ifndef MAPDENSIFY_H
#define MAPDENSIFY_H


namespace Ilwis {
namespace RasterOperations {

class DensifyRaster : public OperationImplementation
{
public:
    DensifyRaster();
    DensifyRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    ~DensifyRaster();

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);
    static quint64 createMetadata();

private:
    bool isValueMap();
    bool isItemMap();

    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    double _enlargementFactor;
    Ilwis::RasterInterpolator::InterpolationMethod _interpolationMethod;

    NEW_OPERATION(DensifyRaster);
};

}
}

#endif // MAPDENSIFY_H
