#ifndef CROSSRASTERS_H
#define CROSSRASTERS_H

namespace Ilwis {
namespace RasterOperations {


class CrossRastersBase : public OperationImplementation
{
public:
    enum UndefHandling { uhIgnoreUndef1, uhIgnoreUndef2, uhIgnoreUndef, uhDontCare};
    CrossRastersBase();

    CrossRastersBase(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata(Ilwis::OperationResource &operation);

protected:
    IRasterCoverage _inputRaster1;
    IRasterCoverage _inputRaster2;
    ITable _outputTable;
    IRasterCoverage _outputRaster;
    INamedIdDomain _crossDomain;
    UndefHandling _undefhandling;

    bool crossNoRaster(const BoundingBox& box);
    bool crossWithRaster(const  BoundingBox& box);
    QString determineCrossId(double v1, double v2) const;
    void checkUndef(double &v1, double &v2);
};

class CrossRastersWithRasterOutput : public CrossRastersBase
{
public:
   CrossRastersWithRasterOutput(quint64 metaid, const Ilwis::OperationExpression &expr);

   static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

   static quint64 createMetadata();

    NEW_OPERATION(CrossRastersWithRasterOutput);
};

class CrossRasters : public CrossRastersBase
{
public:
    CrossRasters(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);

    static quint64 createMetadata();

    NEW_OPERATION(CrossRasters);
};
}
}

#endif // CROSSRASTERS_H
