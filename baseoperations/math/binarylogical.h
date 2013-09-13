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

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    State prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &);
private:
    bool executeCoverageNumber(Ilwis::ExecutionContext *ctx, SymbolTable& symTable);
    bool executeCoverageCoverage(Ilwis::ExecutionContext *ctx, SymbolTable& symTable);
    bool prepareCoverageCoverage();
    bool prepareCoverageNumber(IlwisTypes ptype1, IlwisTypes ptype2);
    bool setOutput(ExecutionContext *ctx, SymbolTable& symTable);

    bool _coveragecoverage;
    IRasterCoverage _inputGC1;
    IRasterCoverage _inputGC2;
    IRasterCoverage _outputGC;
    double _number;
    Box3D<qint32> _box;
    OperatorType _operator;
};
}
}

#endif // BLOGICAL_H
