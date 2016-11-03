#ifndef AggregateRaster_H
#define AggregateRaster_H

namespace Ilwis {
namespace RasterOperations {
class AggregateRaster : public OperationImplementation
{
public:
    AggregateRaster();
    AggregateRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();


private:
    quint32 groupSize(int dim=0);
    IIlwisObject _inputObj;
    IIlwisObject _outputObj;
    IGeoReference _targetGrf;
    NumericStatistics::PropertySets _method;
    bool _grouped;
    std::vector<quint32> _groupSize = {1,1,1};

    NumericStatistics::PropertySets toMethod(const QString &nm);
    void executeGrouped(const BoundingBox &inpBox);
    void executeNonGrouped(const BoundingBox &inpBox);
};
}
}

#endif // AggregateRaster_H
