#ifndef BLOGICAL_H
#define BLOGICAL_H

namespace Ilwis {
namespace BaseOperations{
class BinaryLogical : public OperationImplementation
{
public:
    enum OperatorType{otNONE,otAND, otOR, otXOR, otLESS, otLESSEQ, otNEQ, otEQ, otGREATER, otGREATEREQ};
    BinaryLogical();
    BinaryLogical(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    State prepare();
private:
    bool executeCoverageNumber(Ilwis::ExecutionContext *ctx);
    bool executeCoverageCoverage(Ilwis::ExecutionContext *ctx);
    bool prepareCoverageCoverage();
    bool prepareCoverageNumber(IlwisTypes ptype1, IlwisTypes ptype2);
    bool setOutput(ExecutionContext *ctx);

    bool _coveragecoverage;
    IGridCoverage _inputGC1;
    IGridCoverage _inputGC2;
    IGridCoverage _outputGC;
    double _number;
    Box3D<qint32> _box;
    OperatorType _operator;
};
}
}

#endif // BLOGICAL_H
