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

    IWorkflow workflow = (IWorkflow)_metadata;
    QList<OVertex> inputNodes = workflow->getNodesWithExternalInput();

    quint16 inputParamIndex = 0;
    for (OVertex inputNode : inputNodes) {
        QStringList requireds;
        QStringList optionals;
        IOperationMetaData meta = workflow->getOperationMetadata(inputNode);
        meta->parametersFromSyntax(requireds,optionals);

        QStringList arguments;
        for (int i = 0 ; i < requireds.size() ; i++) {
            arguments << _expression.parm(inputParamIndex).value();
            inputParamIndex++;
        }
        for (int i = 0 ; i < optionals.size() ; i++) {
            if (inputParamIndex < _expression.parameterCount()) {
                arguments << _expression.parm(inputParamIndex).value();
                inputParamIndex++;
            }
        }

        _inputArgs.insert(inputNode, arguments);
    }


    if ( !inputNodes.isEmpty()) {
        executeFromInputNode(inputNodes.at(0), ctx, symTable);
    }


    // execution traversal over edges

    QList<OVertex> outputNodes = workflow->getNodesWithExternalOutputs();

    // TODO

    return true;
}


bool WorkflowOperationImplementation::executeFromInputNode(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable)
{
    QStringList requireds;
    QStringList optionals;
    IWorkflow workflow = (IWorkflow)_metadata;
    IOperationMetaData meta = workflow->getOperationMetadata(v);
    meta->parametersFromSyntax(requireds,optionals);

    QStringList outputParameters;
    for (int i = 0 ; i < meta->getOutputParameters().size() ; i++) {
        SPOperationParameter parameter = meta->getOutputParameters().at(i);
        QString outputterm = parameter->term().isEmpty()
                ? QString("pout_%1_%2").arg(v).arg(i)
                : parameter->term() ;
        outputParameters << outputterm;
    }


    QString execString = QString("%1%2(%3)"); // TODO named arguments
    if ( !outputParameters.isEmpty()) {
        execString = execString.arg(outputParameters.join(", ") + "=");
    }
    execString = execString.arg(meta->name());
    execString = execString.arg(_inputArgs[v].join(", "));

    qDebug() << "execute node " << v << " via: " << execString;

    bool ok = commandhandler()->execute(execString, ctx, symTable);
    if ( !ok) {
        ERROR1("workflow execution failed when executing: ", execString);
    }



}
