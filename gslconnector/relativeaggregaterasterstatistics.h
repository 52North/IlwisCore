#ifndef RelativeAggregateRasterStatistics_H
#define RelativeAggregateRasterStatistics_H

namespace Ilwis {
namespace GSL {

typedef std::function<double (const double data[], size_t stride, size_t n, double mean)> StatFunctionModel2;

class RelativeAggregateRasterStatistics : public OperationImplementation
{
public:
    RelativeAggregateRasterStatistics();

    RelativeAggregateRasterStatistics(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

   NEW_OPERATION(RelativeAggregateRasterStatistics);

private:
    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    IRasterCoverage _relativeFromRaster;
    QString _operationName;
    StatFunctionModel2 _statisticsFunction;
};
}
}

#endif // RelativeAggregateRasterStatistics_H
