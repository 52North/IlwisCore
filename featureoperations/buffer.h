#ifndef BUFFER_H
#define BUFFER_H

namespace Ilwis {
namespace FeatureOperations {
class Buffer : public OperationImplementation
{
public:

    Buffer();

    Buffer(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

private:
    IFeatureCoverage _inputFeatures;
    IFeatureCoverage _outputFeatures;

    double _distance;
    quint32 _nQuadrantSegments;
    quint32 _endCapStyle;

    NEW_OPERATION(Buffer);

    void addAttributeColumns();
    void copyAttributeValues(SPFeatureI &outputFeature, SPFeatureI &inputFeature);

};
}
}

#endif // BUFFER_H
