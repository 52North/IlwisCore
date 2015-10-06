#ifndef POLYGONTORASTER_H
#define POLYGONTORASTER_H

namespace Ilwis {
namespace RasterOperations {

class PolygonToRaster : public OperationImplementation
{
public:
    PolygonToRaster();
    ~PolygonToRaster();
    PolygonToRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IFeatureCoverage _inputfeatures;
    IRasterCoverage _outputraster;
    IGeoReference _inputgrf;
    Envelope env;
    bool _needCoordinateTransformation = false;

    NEW_OPERATION(PolygonToRaster);
};
}
}

#endif // POLYGONTORASTER_H
