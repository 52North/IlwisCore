#ifndef WORKFLOWOPERATIONIMPLEMENTATION_H
#define WORKFLOWOPERATIONIMPLEMENTATION_H

namespace Ilwis {
class WorkflowOperationImplementation: public OperationImplementation
{
public:
    WorkflowOperationImplementation();
    ~WorkflowOperationImplementation();

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

protected:
    WorkflowOperationImplementation(quint64 metaid, const Ilwis::OperationExpression &expr);



};
}

#endif // WORKFLOWOPERATIONIMPLEMENTATION_H
