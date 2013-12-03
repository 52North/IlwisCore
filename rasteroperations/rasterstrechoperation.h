#ifndef RASTERSTRECHOPERATION_H
#define RASTERSTRECHOPERATION_H

namespace Ilwis {
namespace RasterOperations {

class LinearStretchOperation : public OperationImplementation
{
public:
    LinearStretchOperation();
    LinearStretchOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();


protected:
    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    std::pair<double, double> _limits = std::pair<double,double>(rUNDEF,rUNDEF);

    bool stretch(IRasterCoverage toStretch);
};

class HistogramEqualizationOperation : public OperationImplementation
{
public:
    HistogramEqualizationOperation();
    HistogramEqualizationOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

protected:
    IRasterCoverage _stretchFrom;
    IRasterCoverage _stretchTo;


};

}
}

#endif // RASTERSTRECHOPERATION_H
