#ifndef AGGREGATERASTERSTATISTICS_H
#define AGGREGATERASTERSTATISTICS_H

namespace Ilwis {
namespace RasterOperations {


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
    IRasterCoverage _relativeFromRaster;
    QString _operation;
};
}
}

#endif // AGGREGATERASTERSTATISTICS_H
