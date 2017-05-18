#ifndef SETVALUERANGE_H
#define SETVALUERANGE_H

namespace Ilwis {
namespace BaseOperations {

class SetValueRange : public OperationImplementation
{
public:
    SetValueRange();
    SetValueRange(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);

    static quint64 createMetadata();
private:
    IRasterCoverage _raster;
    IRasterCoverage _outputRaster;
    BoundingBox _box;
    double _min = rUNDEF;
    double _max = rUNDEF;
    double _step = rUNDEF;

    NEW_OPERATION(SetValueRange)
};
}
}

#endif // SETVALUERANGE_H
