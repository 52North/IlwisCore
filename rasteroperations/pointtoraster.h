#ifndef POINTTORASTER_H
#define POINTTORASTER_H


namespace Ilwis {
namespace RasterOperations {

class PointToRaster : public OperationImplementation
{
public:
    PointToRaster();
    ~PointToRaster();
    PointToRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IFeatureCoverage _inputfeatures;
    IRasterCoverage _outputraster;
    IGeoReference _inputgrf;

    NEW_OPERATION(PointToRaster);
};
}
}
#endif // POINTTORASTER_H
