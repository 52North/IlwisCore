#ifndef FILTERRASTER_H
#define FILTERRASTER_H

namespace Ilwis {
namespace RasterOperations {

class LinearRasterFilter : public OperationImplementation
{
public:
    LinearRasterFilter();

    LinearRasterFilter(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    std::unique_ptr<RasterFilter> _filter;

    NEW_OPERATION(LinearRasterFilter);
};
}
}

#endif // FILTERRASTER_H
