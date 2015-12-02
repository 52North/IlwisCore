#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "raster.h"
#include "featurecoverage.h"
#include "table.h"

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
    for (int i = 0; i<outputNodes.size(); ++i) {
        ExecutionContext ctx;
        SymbolTable symTable;
        bool ok = reverseFollowExecutionPath(outputNodes[i], &ctx, symTable);
        if (!ok) {
            ERROR0("workflow execution failed when executing!");
            return false;
        }

        //for (int i = 0 ; i < _expression.parameterCount(false) ; i++) {
        Parameter parameter = _expression.parm(i, false);
        Symbol symbol = symTable.getSymbol(ctx._results[0]);
        copyToContext(symbol, parameter.value(), globalCtx, globalSymTable);
        //}
    }
    return true;
}

void WorkflowOperationImplementation::copyToContext(const Symbol &symbol, const QString &name, ExecutionContext *ctx, SymbolTable &symTable)
{
    Resource resource;
    IlwisTypes tp = symbol._type;
    if ( tp & itRASTER) {
        IIlwisObject o = symbol._var.value<IRasterCoverage>();
        resource = o->source();
        o->name(name);
    }
    if ( tp & itFEATURE) {
        IIlwisObject o = symbol._var.value<IFeatureCoverage>();
        resource = o->source();
        o->name(name);
    }
    if ( hasType(tp , itTABLE)) {
        IIlwisObject o = symbol._var.value<ITable>();
        resource = o->source();
        o->name(name);
    }
    ctx->addOutput(symTable, symbol._var, name, symbol._type, resource);
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
        for (int i = 0 ; i < meta->inputParameterCount() ; i++) {
            arguments.push_back(""); // initialize empty arguments
        }

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
                    if (inputData->value.size() > 0) {
                        // constant input value
                        argument = inputData->value;
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
                quint16 optionalIndex = requireds.size();
                if ( !workflow->hasInputAssignment(inputNode, optionalIndex)) {
                    continue; // implicit or non-existent input
                } else {
                    SPAssignedInputData inputData = workflow->getAssignedInputData({inputNode, optionalIndex});
                    SPOperationParameter parameter = parameters.at(optionalIndex);
                    QString namedOptional = "%1";/*parameter->term() + "=%1"*/ // TODO named optionals
                    if (inputs.contains(inputData)) {
                        // shared over multiple operations
                        arguments.insert(optionalIndex, namedOptional.arg(inputs.value(inputData)));
                    } else {
                        QString argument;
                        if (inputData->value.size() > 0) {
                            // constant input value
                            argument = inputData->value;
                        } else {
                            argument = _expression.parm(inputParamIndex).value();
                            inputParamIndex++;
                        }
                        inputs.insert(inputData, argument);
                        arguments.insert(optionalIndex, namedOptional.arg(argument));
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
    QString argumentlist = _inputArgs[v].join(",").remove(QRegExp(",+$"));
    execString = execString.arg(argumentlist);
    qDebug() << "executing input node" << execString;
    bool ok = commandhandler()->execute(execString, ctx, symTable);
    if ( !ok) {
        ERROR1("workflow execution failed when executing: %1", execString);
    } else {
        _nodeExecutionContext[v] = std::make_pair(ctx, symTable);
    }
    return ok;
}

bool WorkflowOperationImplementation::reverseFollowExecutionPath(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_nodeExecutionContext.contains(v)) {
        return true;
    }
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

    for (InputAssignment assignment : workflow->getConstantInputAssignments(v)) {
        SPAssignedInputData input = workflow->getAssignedInputData(assignment);
        arguments.insert(assignment.second, input->value);
    }

    ExecutionContext localCtx;
    SymbolTable localSymTable;
    //InEdgeIterator ei, ei_end;
    for (/*boost::tie(ei,ei_end) = workflow->getInEdges(v)*/; ei != ei_end; ++ei) {
        OVertex previous = workflow->getSourceOperationNode(*ei);
        ok = reverseFollowExecutionPath(previous, &localCtx, localSymTable);
        if ( !ok) {
            return false;
        } else {
            std::vector<SPOperationParameter> outputs = meta->getOutputParameters();
            EdgeProperties edgeProperties = workflow->edgeProperties(*ei);

            quint16 inIdx = edgeProperties._inputParameterIndex;
            quint16 outIdx = edgeProperties._outputParameterIndex;
            QString resultName = localCtx._results[outIdx];
            Symbol tmpResult = localSymTable.getSymbol(resultName);
            //copyToContext(tmpResult, resultName, ctx, symTable);

            if (hasType(itILWISOBJECT, tmpResult._type)) {
                // anonymous objects have to be resolved
                arguments.insert(inIdx, resultName);
            } else {
                // simple types can be passed in directly
                arguments.insert(inIdx, tmpResult._var.toString());
            }

            if ( !edgeProperties.temporary) {
                QString outputName = edgeProperties.outputName.isEmpty()
                        ? QString("%1_node%2_pout%3").arg(workflow->name()).arg(v).arg(outIdx)
                        : edgeProperties.outputName;

                Symbol symbol = symTable.getSymbol(outputName);
                if (symbol.isValid()) {
                    // make unique in shared execution context/symbol table
                    outputName = QString("%1_node%2_pout%3_%4").arg(workflow->name()).arg(v).arg(outIdx).arg(outputName);
                }
                copyToContext(symbol, outputName, ctx, symTable);
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

    QString argumentlist = arguments.join(",").remove(QRegExp(",+$"));
    QString execString = QString("%1(%2)").arg(meta->name()).arg(argumentlist);
    qDebug() << "executing " << execString;
    bool success = commandhandler()->execute(execString, ctx, symTable);
    if ( !success) {
        ERROR1("workflow execution failed when executing: %1", execString);
    } else {
        _nodeExecutionContext[v] = std::make_pair(ctx, symTable);
    }
    return success;

}

