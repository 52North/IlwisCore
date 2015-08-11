#ifndef RASTERTOPOINT_H
#define RASTERTOPOINT_H


namespace Ilwis {
namespace RasterOperations {

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

    NEW_OPERATION(RasterToPoint);
};
}
}


#endif // RASTERTOPOINT_H
