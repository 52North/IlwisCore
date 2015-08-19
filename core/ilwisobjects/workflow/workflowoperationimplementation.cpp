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


bool WorkflowOperationImplementation::execute(ExecutionContext *globalCtx, SymbolTable &globalSymTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(globalCtx,globalSymTable)) != sPREPARED)
            return false;

    /*
     * inputs/outputs are cached by the workflow so they have same
     * order which has been used when creating the metadata syntax
     */
    IWorkflow workflow = (IWorkflow) _metadata;
    QList<OVertex> inputNodes = workflow->getNodesWithExternalInput();
    parseInputNodeArguments(inputNodes, workflow);

    //if ( !inputNodes.isEmpty()) {
    //    executeFromInputNode(inputNodes.at(0), ctx, symTable);
    //}

    QList<OVertex> outputNodes = workflow->getNodesWithExternalOutputs();
    for (OVertex outputNode : outputNodes) {
        if ( !reverseFollowExecutionPath(outputNode, globalCtx, globalSymTable)) {
            ERROR0("workflow execution failed when executing!");
            return false;
        }
    }

    // TODO

    return true;
}

void WorkflowOperationImplementation::parseInputNodeArguments(const QList<OVertex> &inputNodes, const IWorkflow &workflow)
{
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

        std::vector<SPOperationParameter> parameters = meta->getInputParameters();
        for (int i = 0 ; i < optionals.size() ; i++) {
            if (inputParamIndex < _expression.parameterCount()) {
                SPOperationParameter parameter = parameters.at(requireds.size() + i);
                QString arg = _expression.parm(inputParamIndex).value();
                QString namedOptional = parameter->term() + "=" + arg;

                // TODO named optionals
                // arguments << namedOptional;
                arguments << arg;
                inputParamIndex++;
            }
        }

        _inputArgs.insert(inputNode, arguments);
    }
}


bool WorkflowOperationImplementation::executeInputNode(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable)
{
    QStringList requireds;
    QStringList optionals;
    IWorkflow workflow = (IWorkflow)_metadata;
    IOperationMetaData meta = workflow->getOperationMetadata(v);
    meta->parametersFromSyntax(requireds,optionals);

    /*
    QStringList outputParameters;
    for (int i = 0 ; i < meta->getOutputParameters().size() ; i++) {
        SPOperationParameter parameter = meta->getOutputParameters().at(i);
        QString outputterm = parameter->term().isEmpty()
                ? QString("pout_%1_%2").arg(v).arg(i)
                : parameter->term() ;
        outputParameters << outputterm;
    }
    */

    QString execString = QString("%1(%2)");
    /*
    if ( !outputParameters.isEmpty()) {
        execString = execString.arg(outputParameters.join(", ") + "=");
    }
    */
    execString = execString.arg(meta->name());
    execString = execString.arg(_inputArgs[v].join(", "));

    bool ok = commandhandler()->execute(execString, ctx, symTable);
    if ( !ok) {
        ERROR1("workflow execution failed when executing: ", execString);
    }
    return ok;

    OutEdgeIterator ei, ei_end;
    for (boost::tie(ei,ei_end) = workflow->getOutEdges(v); ei != ei_end; ++ei) {
        EdgeProperties properties = workflow->edgeProperties(*ei);
        quint16 outIdx = properties.outputIndexLastOperation;
        quint16 inIdx = properties.inputIndexNextOperation;

        QVariant outValue = symTable.getValue(ctx->_results[outIdx]);

        OVertex nextOperation = workflow->getNextOperationNode(*ei);
    }

}

bool WorkflowOperationImplementation::reverseFollowExecutionPath(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable)
{
    IWorkflow workflow = (IWorkflow)_metadata;

    InEdgeIterator ei, ei_end;
    boost::tie(ei,ei_end) = workflow->getInEdges(v);

    if (ei == ei_end/*workflow->isInputNode(v)*/) {
        return executeInputNode(v, ctx, symTable);
    }

    bool ok;
    IOperationMetaData meta = workflow->getOperationMetadata(v);
    QStringList arguments;

    for (SPInputDataProperties input : workflow->getInputDataProperties(v)) {
        arguments.insert(input->assignedParameterIndex, input->value.toString());
    }

    ExecutionContext localCtx;
    SymbolTable localSymTable;
    //InEdgeIterator ei, ei_end;
    for (/*boost::tie(ei,ei_end) = workflow->getInEdges(v)*/; ei != ei_end; ++ei) {
        OVertex previous = workflow->getPreviousOperationNode(*ei);
        ok = reverseFollowExecutionPath(previous, &localCtx, localSymTable);
        if ( !ok) {
            return false;
        } else {
            std::vector<SPOperationParameter> outputs = meta->getOutputParameters();
            EdgeProperties edgeProperties = workflow->edgeProperties(*ei);

            quint16 inIdx = edgeProperties.inputIndexNextOperation;
            quint16 outIdx = edgeProperties.outputIndexLastOperation;
            QString resultName = localCtx._results[outIdx];
            QVariant result = localSymTable.getValue(resultName);

            // TODO named optionals
            // arguments << namedOptional;
            arguments.insert(inIdx, result.toString()); // type does not matter

            if ( !edgeProperties.temporary) {
                QString outputName = edgeProperties.outputName.isEmpty()
                        ? QString("%1_node%2_pout%3").arg(workflow->name()).arg(v).arg(outIdx)
                        : edgeProperties.outputName;

                if (symTable.getSymbol(outputName).isValid()) {
                    // make unique in shared execution context/symbol table
                    outputName = QString("%1_node%2_pout%3_%4").arg(workflow->name()).arg(v).arg(outIdx).arg(outputName);
                }

                Resource resource; // TODO handle correctly?!
                SPOperationParameter parameter = outputs.at(outIdx);
                ctx->addOutput(symTable, result, outputName, parameter->type(), resource);
            }
        }
    }

    QString execString = QString("%1(%2)").arg(meta->name()).arg(arguments.join(","));
    return commandhandler()->execute(execString, ctx, symTable);

}

