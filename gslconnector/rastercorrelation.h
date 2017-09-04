#ifndef RasterCorrelation_H
#define RasterCorrelation_H

namespace Ilwis {
namespace GSL {

typedef std::function<double (const double data[], size_t , size_t)> StatFunctionModel1;

class RasterCorrelation : public OperationImplementation
{
public:
    RasterCorrelation();

    RasterCorrelation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

   NEW_OPERATION(RasterCorrelation);

private:
    IRasterCoverage _inputRaster1;
    IRasterCoverage _inputRaster2;
    IRasterCoverage _outputRaster;

};
}
}

#endif // RasterCorrelation_H
