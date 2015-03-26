#include "kernel.h"
#include "ilwisdata.h"
#include "ilwisobject.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operationExpression.h"

#include "workflowexecutionengine.h"


using namespace Ilwis;

WorkflowExecutionEngine::WorkflowExecutionEngine()
{
}

WorkflowExecutionEngine::WorkflowExecutionEngine(quint64 metaid, const OperationExpression &expr)
{

}

OperationImplementation *WorkflowExecutionEngine::create(quint64 metaid, const OperationExpression &expr)
{

    // TODO

}

bool WorkflowExecutionEngine::execute(ExecutionContext *ctx, SymbolTable &symTable)
{

}

OperationImplementation::State WorkflowExecutionEngine::prepare(ExecutionContext *ctx, const SymbolTable &)
{

}

