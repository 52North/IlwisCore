#ifndef POINTRASTERCROSSING_H
#define POINTRASTERCROSSING_H

namespace Ilwis {
namespace FeatureOperations {


class PointRasterCrossing : public OperationImplementation
{
public:
    PointRasterCrossing();
    PointRasterCrossing(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    State prepare(ExecutionContext *ctx, const SymbolTable& sym);

private:
    IRasterCoverage _inputRaster;
    IFeatureCoverage _inputFeatures;
    IFeatureCoverage _outputFeatures;
    bool _doCoordTransform;
    quint32 _startColumn;
    QString _prefix;

    NEW_OPERATION(PointRasterCrossing);
    QString columnName(const QVariant &trackIndexValue);
    void addAttributes(const ITable &inputTable);
};
}
}

#endif // POINTRASTERCROSSING_H
