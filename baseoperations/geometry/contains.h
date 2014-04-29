#ifndef CONTAINS_H
#define CONTAINS_H

namespace Ilwis {
namespace BaseOperations {
class Contains : public OperationImplementation
{
public:
    Contains();

    Contains(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

   NEW_OPERATION(Contains);

private:
    ICoverage _coverage;
    IFeatureCoverage _outputFeatures;
    bool _containsWKT;
    std::unique_ptr<geos::geom::Geometry> _geometry;
};
}
}

#endif // CONTAINS_H
