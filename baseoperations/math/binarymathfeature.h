#ifndef BINARYMATHFEATURE_H
#define BINARYMATHFEATURE_H

namespace Ilwis {
namespace BaseOperations{
class BinaryMathFeature : public OperationImplementation
{
public:
    enum OperatorType{ otPLUS, otMINUS};
    BinaryMathFeature();
    BinaryMathFeature(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    State prepare(ExecutionContext *ctx, const SymbolTable& sym);

private:
    IFeatureCoverage _inputFeatureSet1;
    IFeatureCoverage _inputFeatureSet2;
    IFeatureCoverage _outputFeatures;
    ICoordinateSystem _csyTarget;
    RenumberMap _renumberer;
    TableMerger _merger;
    Envelope addEnvelopes() const;

    NEW_OPERATION(BinaryMathFeature);
};
}
}

#endif // BINARYMATHFEATURE_H
