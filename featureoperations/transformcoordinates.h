#ifndef TRANSFORMCOORDINATES_H
#define TRANSFORMCOORDINATES_H

namespace Ilwis {
namespace FeatureOperations {


class TransformCoordinates : public OperationImplementation
{
public:
    TransformCoordinates();

    TransformCoordinates(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    State prepare(ExecutionContext *ctx, const SymbolTable& sym);

    NEW_OPERATION(TransformCoordinates);

private:
    IFeatureCoverage _inputFeatures;
    IFeatureCoverage _outputFeatures;
    ICoordinateSystem _targetCsy;


};
}
}

#endif // TRANSFORMCOORDINATES_H
