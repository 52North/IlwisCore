#ifndef CROSSRASTERS_H
#define CROSSRASTERS_H

namespace Ilwis {
namespace RasterOperations {

class CrossRasters : public OperationImplementation
{
public:
    CrossRasters();

    CrossRasters(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();
};
}
}

#endif // CROSSRASTERS_H
