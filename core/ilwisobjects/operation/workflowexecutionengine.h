#ifndef WORKFLOWEXECUTIONENGINE_H
#define WORKFLOWEXECUTIONENGINE_H


#include "operation.h"

namespace Ilwis {


class WorkflowExecutionEngine : OperationImplementation
{
public:
    WorkflowExecutionEngine();
    WorkflowExecutionEngine(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

};

}
#endif // WORKFLOWEXECUTIONENGINE_H
