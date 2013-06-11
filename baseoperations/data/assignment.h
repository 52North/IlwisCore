#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

namespace Ilwis {
namespace BaseOperations {

class Assignment : public OperationImplementation
{
public:
    Assignment();
    Assignment(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx=0);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare();

    static quint64 createMetadata();
private:
    IGridCoverage _inputGC;
    IGridCoverage _outputGC;
};
}
}

#endif // ASSIGNMENT_H
