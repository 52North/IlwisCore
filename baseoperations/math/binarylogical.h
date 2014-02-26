#ifndef BLOGICAL_H
#define BLOGICAL_H

namespace Ilwis {
namespace BaseOperations{
class BinaryLogical : public OperationImplementation
{
public:
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
    BoundingBox _box;
    LogicalOperator _operator;

    NEW_OPERATION(BinaryLogical);
};
}
}

#endif // BLOGICAL_H
