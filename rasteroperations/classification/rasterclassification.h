#ifndef RASTERCLASSIFICATION_H
#define RASTERCLASSIFICATION_H

namespace Ilwis {
namespace RasterOperations {

class Classifier;

class RasterClassification : public OperationImplementation
{
public:
    RasterClassification();
    RasterClassification(quint64 metaid, const Ilwis::OperationExpression &expr);
    bool execute(ExecutionContext *ctx,SymbolTable& symTable);

protected:
    SampleSet _sampleSet;
    IRasterCoverage _outputRaster;
    std::unique_ptr<Classifier> _classifier;
    Ilwis::OperationImplementation::State prepare(ExecutionContext *, const SymbolTable &sym);

    static int fillOperationMetadata(OperationResource& operation);
};

class BoxClassification : public RasterClassification {
public:
    BoxClassification();
    BoxClassification(quint64 metaid, const Ilwis::OperationExpression &expr);

    //bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &sym);

    static quint64 createMetadata();

    NEW_OPERATION(BoxClassification);

private:
    double _widenFactor;
};
}
}

#endif // RASTERCLASSIFICATION_H
