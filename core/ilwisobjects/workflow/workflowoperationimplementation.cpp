#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"

#include "workflowoperationimplementation.h"
#include "workflow.h"

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
    if (_metadata->ilwisType() != itWORKFLOW) {
        ERROR0("Expected type itWORKFLOW here!");
        return sPREPAREFAILED;
    }

    Workflow *workflow = static_cast<Workflow*>(_metadata.ptr());

    QStringList required;
    QStringList optionals;
    workflow->parametersFromSyntax(required, optionals);

    if (_expression.parameterCount() < required.size()) {
        QString list = required.join(", ");
        ERROR1("Number of required paramaters do not match! Required are: %2", list);
        return sPREPAREFAILED;
    }
    std::vector<SPOperationParameter> inputs = workflow->getInputParameters();
    std::vector<SPOperationParameter> outputs = workflow->getOutputParameters();

    // TODO do more preparation and validation checks

    _prepState = sPREPARED;
    return _prepState;
}

bool WorkflowOperationImplementation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    Workflow *workflow = static_cast<Workflow*>(_metadata.ptr());
    QList<OVertex> inputNodes = workflow->getNodesWithExternalInput();

    // execution traversal over edges

    QList<OVertex> outputNodes = workflow->getNodesWithExternalOutputs();

    // TODO

    return true;
}
