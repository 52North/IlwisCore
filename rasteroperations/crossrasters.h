#ifndef CROSSRASTERS_H
#define CROSSRASTERS_H

namespace Ilwis {
namespace RasterOperations {


class CrossRasters : public OperationImplementation
{
public:
    enum UndefHandling { uhIgnoreUndef1, uhIgnoreUndef2, uhIgnoreUndef, uhDontCare};
    CrossRasters();

    CrossRasters(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IRasterCoverage _inputRaster1;
    IRasterCoverage _inputRaster2;
    ITable _outputTable;
    IRasterCoverage _outputRaster;
    INamedIdDomain _crossDomain;
    UndefHandling _undefhandling;

    bool crossNoRaster(const Box3D<qint32> box);
    bool crossWithRaster(const Box3D<qint32> box);
};
}
}

#endif // CROSSRASTERS_H
