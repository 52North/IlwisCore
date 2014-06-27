#ifndef TRANSPOSERASTER_H
#define TRANSPOSERASTER_H

namespace Ilwis {
namespace RasterOperations {

class TransposeRaster : public OperationImplementation
{
public:
    TransposeRaster();

    TransposeRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(TransposeRaster);
};
}
}

#endif // TRANSPOSERASTER_H
