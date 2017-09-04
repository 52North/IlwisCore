#ifndef RasterQuantile_H
#define RasterQuantile_H

namespace Ilwis {
namespace GSL {

typedef std::function<double (const double data[], size_t , size_t)> StatFunctionModel1;

class RasterQuantile : public OperationImplementation
{
public:
    RasterQuantile();

    RasterQuantile(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

   NEW_OPERATION(RasterQuantile);

private:
    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    double _quantile;
};
}
}

#endif // RasterQuantile_H
