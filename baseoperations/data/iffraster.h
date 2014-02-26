#ifndef IFFRASTER_H
#define IFFRASTER_H

namespace Ilwis {
namespace BaseOperations {
class IffRaster : public IfOperation
{
public:
    IffRaster();
    IffRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();
private:
    IRasterCoverage _inputGC;
    IRasterCoverage _outputGC;


    NEW_OPERATION(IffRaster);
};
}
}

#endif // IFFRASTER_H
