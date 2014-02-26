#ifndef GRIDSIZE_H
#define GRIDSIZE_H

namespace Ilwis {
namespace BaseOperations {
class GridSize : public OperationImplementation
{
public:
    GridSize();
    GridSize(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
    OperationImplementation::State prepare(ExecutionContext *, const SymbolTable &);
private:
    IRasterCoverage _inputGC;
    QString _dim;

    NEW_OPERATION(GridSize);

};
}
}

#endif // GRIDSIZE_H
