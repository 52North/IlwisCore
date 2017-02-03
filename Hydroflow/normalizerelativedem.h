#ifndef NORMALIZERELATIVEDEM_H
#define NORMALIZERELATIVEDEM_H

namespace Ilwis {
namespace Hydroflow {

class NormalizeRelativeDEM : public OperationImplementation
{
public:
    NormalizeRelativeDEM();
    NormalizeRelativeDEM(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IIlwisObject _inputDEM;
    IFeatureCoverage _inputCatchment;
    IIlwisObject _outputObj;

    // parameters

    NEW_OPERATION(NormalizeRelativeDEM);
};
}
}

#endif // NORMALIZERELATIVEDEM_H
