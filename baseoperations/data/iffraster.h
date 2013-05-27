#ifndef IFFRASTER_H
#define IFFRASTER_H

namespace Ilwis {
namespace BaseOperations {
class IffRaster : public OperationImplementation
{
public:
    IffRaster();
    IffRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx=0);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare();

    static quint64 createMetadata();
private:
    IGridCoverage _inputGC;
    IGridCoverage _outputGC;
    IGridCoverage _coverages[2];
    double _number[2];

    DataDefinition findOutputDataDef(const OperationExpression &expr) ;
    DataDefinition findParameterDataDef(const OperationExpression &expr, int index) ;
};
}
}

#endif // IFFRASTER_H
