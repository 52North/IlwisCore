#ifndef LINE2RASTER_H
#define LINE2RASTER_H


namespace Ilwis {
namespace RasterOperations {

class Line2Raster : public OperationImplementation
{
public:
    Line2Raster();
    ~Line2Raster();
    Line2Raster(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IFeatureCoverage _inputfeatures;
    IRasterCoverage _outputraster;
    IGeoReference _inputgrf;
    qint32 _primaryColumnIndex = iUNDEF;
    DataDefinition _datadef;

    NEW_OPERATION(Line2Raster);
};
}
}
#endif // LINE2RASTER_H
