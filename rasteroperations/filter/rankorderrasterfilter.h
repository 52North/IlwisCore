#ifndef RANKORDERFILTERRASTER_H
#define RANKORDERFILTERRASTER_H

namespace Ilwis {
namespace RasterOperations {

class RankOrderRasterFilter : public OperationImplementation
{
public:
    RankOrderRasterFilter();

    RankOrderRasterFilter(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    std::unique_ptr<RasterFilter> _filter;

    NEW_OPERATION(RankOrderRasterFilter);
};
}
}

#endif // RANKORDERFILTERRASTER_H
