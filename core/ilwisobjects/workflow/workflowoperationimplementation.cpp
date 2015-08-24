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

    QList<OVertex> outputNodes = workflow->getNodesWithExternalOutputs();
    for (OVertex outputNode : outputNodes) {
        ExecutionContext ctx;
        SymbolTable symTable;
        if ( !reverseFollowExecutionPath(outputNode, &ctx, symTable)) {
            ERROR0("workflow execution failed when executing!");
            return false;
        }

        for (int i = 0 ; i < _expression.parameterCount(false) ; i++) {
            Resource resource = workflow->source();
            Parameter parameter = _expression.parm(i, false);
            QString name = parameter.value();
            IlwisTypes types = parameter.valuetype();
            QVariant value = symTable.getValue(ctx._results[i]);
            globalCtx->addOutput(globalSymTable, value, name, types, resource);
        }
    }

    return true;
}

void WorkflowOperationImplementation::parseInputNodeArguments(const QList<OVertex> &inputNodes, const IWorkflow &workflow)
{
    // cache inputs, might go into multiple operations
    QMap<SPAssignedInputData, QString> inputs;

    quint16 inputParamIndex = 0;
    for (OVertex inputNode : inputNodes) {
        QStringList requireds;
        QStringList optionals;
        IOperationMetaData meta = workflow->getOperationMetadata(inputNode);
        meta->parametersFromSyntax(requireds,optionals);
        QStringList arguments;

        // ------------ parse required arguments

        for (int i = 0 ; i < requireds.size() ; i++) {
            if ( !workflow->hasInputAssignment(inputNode, i)) {
                continue; // implicit input
            } else {
                SPAssignedInputData inputData = workflow->getAssignedInputData({inputNode, i});
                if (inputs.contains(inputData)) {
                    // shared over multiple operations
                    arguments.insert(i, inputs.value(inputData));
                } else {
                    QString argument;
                    if (inputData->value.isValid()) {
                        // constant input value
                        argument = inputData->value.toString();
                    } else {
                        argument = _expression.parm(inputParamIndex).value();
                        inputParamIndex++;
                    }
                    inputs.insert(inputData, argument);
                    arguments.insert(i, argument);
                }
            }
        }

        // ------------ parse optional arguments

        std::vector<SPOperationParameter> parameters = meta->getInputParameters();
        if (inputParamIndex < _expression.parameterCount()) {
            for (int i = 0 ; i < optionals.size() ; i++) {
                quint16 optionalIndex = requireds.size() + 1;
                if ( !workflow->hasInputAssignment(inputNode, optionalIndex)) {
                    continue; // implicit or non-existent input
                } else {
                    SPAssignedInputData inputData = workflow->getAssignedInputData({inputNode, optionalIndex});
                    SPOperationParameter parameter = parameters.at(optionalIndex);
                    QString namedOptional = "%1";/*parameter->term() + "=%1"*/ // TODO named optionals
                    if (inputs.contains(inputData)) {
                        // shared over multiple operations
                        arguments.insert(i, namedOptional.arg(inputs.value(inputData)));
                    } else {
                        QString argument;
                        if (inputData->value.isValid()) {
                            // constant input value
                            argument = inputData->value.toString();
                        } else {
                            argument = _expression.parm(inputParamIndex).value();
                            inputParamIndex++;
                        }
                        inputs.insert(inputData, argument);
                        arguments.insert(i, namedOptional.arg(argument));
                    }
                }
            }
        }

        // ------------ cache to be available when node gets executed

        _inputArgs.insert(inputNode, arguments);
    }
}


bool WorkflowOperationImplementation::executeInputNode(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable)
{
    IWorkflow workflow = (IWorkflow)_metadata;
    IOperationMetaData meta = workflow->getOperationMetadata(v);

    QString execString = QString("%1(%2)");
    execString = execString.arg(meta->name());
    execString = execString.arg(_inputArgs[v].join(","));
    qDebug() << "executing " << execString;
    bool ok = commandhandler()->execute(execString, ctx, symTable);
    if ( !ok) {
        ERROR1("workflow execution failed when executing: %1", execString);
    }
    return ok;
}

bool WorkflowOperationImplementation::reverseFollowExecutionPath(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable)
{
    IWorkflow workflow = (IWorkflow)_metadata;

    InEdgeIterator ei, ei_end;
    boost::tie(ei,ei_end) = workflow->getInEdges(v);

    // has external inputs only
    if (ei == ei_end/*workflow->isInputNode(v)*/) {
        return executeInputNode(v, ctx, symTable);
    }

    bool ok;
    IOperationMetaData meta = workflow->getOperationMetadata(v);
    QStringList arguments;

    for (InputAssignment assignment : workflow->getExplicitInputAssignments(v)) {
        SPAssignedInputData input = workflow->getAssignedInputData(assignment);
        arguments.insert(assignment.second, input->value.toString());
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
            arguments.insert(inIdx, resultName); // type does not matter

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

    QStringList externalInputs = _inputArgs.value(v);
    for (int i = 0 ; i < externalInputs.size() ; i++) {
        QString externalInput = externalInputs.at(i);
        if ( !externalInput.isEmpty()) {
            arguments.insert(i, externalInput);
        }
    }
    QString execString = QString("%1(%2)").arg(meta->name()).arg(arguments.join(","));
    qDebug() << "executing " << execString;
    return commandhandler()->execute(execString, ctx, symTable);

}

