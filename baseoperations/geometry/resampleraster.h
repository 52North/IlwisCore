#ifndef RESAMPLERASTER_H
#define RESAMPLERASTER_H

namespace Ilwis {
namespace BaseOperations {
class ResampleRaster : public OperationImplementation
{
public:
    ResampleRaster();
    ResampleRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();


private:
    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    IGeoReference _targetGrf;
    Ilwis::RasterInterpolator::InterpolationMethod _method;

    NEW_OPERATION(ResampleRaster);

};
}
}

#endif // RESAMPLERASTER_H
