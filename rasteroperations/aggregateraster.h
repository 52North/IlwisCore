#ifndef AggregateRaster_H
#define AggregateRaster_H

namespace Ilwis {
namespace RasterOperations {
class AggregateRaster : public OperationImplementation
{
public:
    AggregateRaster();
    AggregateRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();


private:
    IIlwisObject _inputObj;
    IIlwisObject _outputObj;
    IGeoReference _targetGrf;
    Ilwis::GridInterpolator::InterpolationMethod _method;

};
}
}

#endif // AggregateRaster_H
