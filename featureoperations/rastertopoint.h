#ifndef RASTERTOPOINT_H
#define RASTERTOPOINT_H


namespace Ilwis {
namespace FeatureOperations {

class RasterToPoint : public OperationImplementation
{
public:
    RasterToPoint();
    ~RasterToPoint();
    RasterToPoint(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IFeatureCoverage _outputfeatures;
    IRasterCoverage _inputraster;
    ITable _attTable;
    bool _doCoordTransform  = false;
    ICoordinateSystem _csy;
    IGeoReference _inputgrf;
    NEW_OPERATION(RasterToPoint);
};
}
}
#endif // RASTERTOPOINT_H
