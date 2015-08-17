#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"

#include "workflowoperationimplementation.h"

using namespace Ilwis;

WorkflowOperationImplementation::WorkflowOperationImplementation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

Ilwis::OperationImplementation *WorkflowOperationImplementation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new WorkflowOperationImplementation(metaid, expr);
}

Ilwis::OperationImplementation::State WorkflowOperationImplementation::prepare(ExecutionContext *ctx, const SymbolTable &)
{

}

bool WorkflowOperationImplementation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{

}
