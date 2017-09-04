#ifndef AGGREGATERASTERSTATISTICS_H
#define AGGREGATERASTERSTATISTICS_H

namespace Ilwis {
namespace GSL {

typedef std::function<double (const double data[], size_t , size_t)> StatFunctionModel1;

class AggregateRasterStatistics : public OperationImplementation
{
public:
    AggregateRasterStatistics();

    AggregateRasterStatistics(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

   NEW_OPERATION(AggregateRasterStatistics);

private:
    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    QString _operationName;
    StatFunctionModel1 _statisticsFunction1;
};
}
}

#endif // AGGREGATERASTERSTATISTICS_H
