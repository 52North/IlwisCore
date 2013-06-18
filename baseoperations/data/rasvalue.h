#ifndef RASVALUE_H
#define RASVALUE_H

namespace Ilwis {
namespace BaseOperations {
class RasValue : public OperationImplementation
{
public:
    RasValue();
    RasValue(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx=0);

    static quint64 createMetadata();
private:
    IGridCoverage _inputGC;
    Voxel _vox;

};
}
}

#endif // RASVALUE_H
