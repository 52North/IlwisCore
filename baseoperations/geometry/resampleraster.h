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
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx=0);

    static quint64 createMetadata();


private:
    IGridCoverage _inputGC;
    IGridCoverage _outputGC;
    IGeoReference _targetGrf;
    GridCoverage::InterpolationMethod _method;
    Box3D<qint32> _box;

};
}
}

#endif // RESAMPLERASTER_H
