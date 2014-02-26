#ifndef IFFFEATURE_H
#define IFFFEATURE_H

namespace Ilwis {
namespace BaseOperations {
class IfFeature : public IfOperation
{
public:
    IfFeature();
    IfFeature(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

private:
    IFeatureCoverage _inputFC;
    IFeatureCoverage _outputFC;

    NEW_OPERATION(IfFeature);
};
}
}

#endif // IFFFEATURE_H
