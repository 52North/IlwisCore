#include <QThread>
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

WorkflowOperationImplementation::WorkflowOperationImplementation(QObject *parent) : OperationImplementation()
{

}

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

void WorkflowOperationImplementation::acceptMessage(const QString &type, const QString &subtype, const QVariantMap &parameters)
{
    if ( type == "workflow"){
        Locker<std::mutex> lock(_lock);
        bool ok;
        quint64 id = parameters["id"].toLongLong(&ok);
        if ( ok && id == _metadata->id()){ // check if this was meant for this workflow
            if ( subtype == "stepmode"){
                _stepMode = parameters.contains("id") ? parameters["stepmode"].toBool() : false;
                if ( !_stepMode){
                    _syncMutex.unlock();
                }
            }
        }
    }
}

//void WorkflowOperationImplementation::send(ExecutionContext *ctx, SymbolTable &symTable)
//{
//    if ( !_stepMode)
//        return;
//    QVariantMap data;
//    for(int i=0; i<ctx->_results.size(); ++i){
//        QVariant var = symTable.getValue(ctx->_results[i]);
//        data[QString::number(i)] = var;
//    }
//    emit sendMessage("workflow","outputdata",data);
//}


bool WorkflowOperationImplementation::execute(ExecutionContext *globalCtx, SymbolTable &globalSymTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(globalCtx,globalSymTable)) != sPREPARED)
            return false;

    connect(this, &WorkflowOperationImplementation::sendMessage, kernel(),&Kernel::acceptMessage);
    connect(kernel(), &Kernel::sendMessage, this, &WorkflowOperationImplementation::acceptMessage);

    QThread *current = QThread::currentThread();
    QVariant var = current->property("runparameters");
    if ( var.isValid()){
        QVariantMap values = var.toMap();
        _stepMode = values["stepmode"].toBool();
        if ( _stepMode){
            bool ok;
            quint32 id = values["runid"].toUInt(&ok);
            if ( ok){
                _runid = id;
                kernel()->addSyncLock(_runid);
            }else
                _stepMode = false;
        }
    }
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
    if ( _runid != iUNDEF){
        kernel()->removeSyncLock(_runid)        ;
    }
    _nodeExecutionContext = QMap<OVertex, std::pair<std::vector<QString>, SymbolTable>>();
    return true;
}

void WorkflowOperationImplementation::copyToContext(const Symbol &symbol, const QString &name, ExecutionContext *ctx, SymbolTable &symTable)
{
    Resource resource;
    IlwisTypes tp = symbol._type;
    if ( tp & itRASTER) {
        IIlwisObject o = symbol._var.value<IRasterCoverage>();
        resource = o->resource();
        o->name(name);
    }
    if ( tp & itFEATURE) {
        IIlwisObject o = symbol._var.value<IFeatureCoverage>();
        resource = o->resource();
        o->name(name);
    }
    if ( hasType(tp , itTABLE)) {
        IIlwisObject o = symbol._var.value<ITable>();
        resource = o->resource();
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


void WorkflowOperationImplementation::wait()
{
    if (_stepMode){
        bool ok;
        QWaitCondition& waitc = kernel()->waitcondition(_runid, ok);
        if ( ok){
            _syncMutex.lock();
            waitc.wait(&_syncMutex);
        }
    }
}

void WorkflowOperationImplementation::sendData(const OVertex &v,ExecutionContext *ctx, SymbolTable &symTable)
{
    if ( _stepMode){
        QVariantList data;
        QVariantMap generic;
        generic["id"] = _metadata->id();
        IWorkflow workflow = (IWorkflow)_metadata;
        IOperationMetaData meta = workflow->getOperationMetadata(v);
        for(int i=0; i<ctx->_results.size(); ++i){
            QVariantMap opdata;
            QVariant var = symTable.getValue(ctx->_results[i]);
            opdata["value"] = var;
            IlwisTypes tp = symTable.ilwisType(QVariant(),ctx->_results[i]);
            opdata["type"] = tp;
            opdata["resultname"] = ctx->_results[i];
            opdata["vertex"] = v;
            QString name = QString("%2=%1(%3)").arg(meta->name()).arg(v).arg(i);
            opdata["name"] = name;
            IlwisTypes id = var2id(var, tp);
            Resource res = mastercatalog()->id2Resource(id);
            opdata["id"] = id;

            data.append(opdata);
        }
        generic["results"] = data;
        emit sendMessage("workflow","outputdata",generic);
        _syncMutex.unlock();
    }
}

quint64 WorkflowOperationImplementation::var2id(const QVariant var, IlwisTypes tp) const{
    quint64 id = -1;
    if  ( hasType(tp, itRASTER)){
        IRasterCoverage obj = var.value<IRasterCoverage>();
        if ( obj.isValid())
            id = obj->id();
    }else    if  ( hasType(tp, itFEATURE)){
        IFeatureCoverage obj = var.value<IFeatureCoverage>();
        if ( obj.isValid())
            id = obj->id();
    }else if  ( hasType(tp, itTABLE)){
        ITable obj = var.value<ITable>();
        if ( obj.isValid())
            id = obj->id();
    }else if  ( hasType(tp, itGEOREF)){
        IGeoReference obj = var.value<IGeoReference>();
        if ( obj.isValid())
            id = obj->id();
    }else if  ( hasType(tp, itCOORDSYSTEM)){
        ICoordinateSystem obj = var.value<ICoordinateSystem>();
        if ( obj.isValid())
            id = obj->id();
    }else if  ( hasType(tp, itDOMAIN)){
        IDomain obj = var.value<IDomain>();
        if ( obj.isValid())
            id = obj->id();
    }
    return id;
}

Resource WorkflowOperationImplementation::var2resource(const Symbol& sym){
    quint64 id = var2id(sym._var, sym._type);
    return mastercatalog()->id2Resource(id);
}

bool WorkflowOperationImplementation::executeInputNode(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable)
{
    IWorkflow workflow = (IWorkflow)_metadata;
    IOperationMetaData meta = workflow->getOperationMetadata(v);

    QString execString = QString("%1(%2)");
    execString = execString.arg(meta->name());
    QString outputs;
    for(int i = 0; i < meta->outputParameterCount(); ++i){
        if ( outputs != "")
            outputs += ",";
        else
            outputs = meta->name();
        outputs += "_" + QString::number(v) + "_" + QString::number(i);
    }
    QString argumentlist = _inputArgs[v].join(",").remove(QRegExp(",+$"));
    execString = execString.arg(argumentlist);
    //execString = outputs + "=" + execString;

    QVariantMap mp;
    mp["vertex"] = v;
    mp["id"] = _metadata->id();
    emit sendMessage("workflow","currentvertex",mp);

    wait();
   // qDebug() << "executing input node" << execString;
    bool ok = commandhandler()->execute(execString, ctx, symTable);


    sendData(v, ctx, symTable);
    if ( !ok) {
        ERROR1("workflow execution failed when executing: %1", execString);
    } else {
        _nodeExecutionContext[v] = std::make_pair(ctx->_results, symTable);
    }
    return ok;
}

bool WorkflowOperationImplementation::doCondition(const IOperationMetaData& meta,const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable){
    bool conditionsResult = checkConditions(v, ctx, symTable);

    // IF conditions are not met
    if (!conditionsResult) {
        // Return empty outputs and DON'T execute this operation.
        QString resultName = CONDITION_FAILED;
        for (int i = 0; i < meta->getOutputParameters().size(); ++i) {
            ExecutionContext localCtx;
            SymbolTable localSymTable;
            localCtx.addOutput(localSymTable, QVariant(), resultName, itSTRING, Resource());
            copyToContext(localSymTable.getSymbol(localCtx._results[i]), resultName, ctx, symTable);
        }
        return true; // Not false because it is valid for conditions to fail.
    }
    return false;
}

bool WorkflowOperationImplementation::reverseFollowExecutionPath(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_nodeExecutionContext.contains(v)) {
        auto &value = _nodeExecutionContext[v];
        ctx->_results = value.first;
        symTable = value.second;
        return true;
    }
    IWorkflow workflow = (IWorkflow)_metadata;
    IOperationMetaData meta = workflow->getOperationMetadata(v);

    InEdgeIterator ei, ei_end;
    boost::tie(ei,ei_end) = workflow->getInEdges(v);

    if( doCondition(meta, v,ctx,symTable))
        return true;

    // has external inputs only
    if (ei == ei_end/*workflow->isInputNode(v)*/) {
        return executeInputNode(v, ctx, symTable);
    }

    bool ok;
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
            if ( outIdx >= localCtx._results.size())
                return false;
            QString resultName = localCtx._results[outIdx];
            Symbol tmpResult = localSymTable.getSymbol(resultName);
            //copyToContext(tmpResult, resultName, ctx, symTable);

            if (resultName != CONDITION_FAILED) {
                if (hasType(itILWISOBJECT, tmpResult._type)) {
                    // anonymous objects have to be resolved
                    Resource res = var2resource(tmpResult);
                    arguments.insert(inIdx, res.url().toString());
                } else {
                    // simple types can be passed in directly
                    arguments.insert(inIdx, tmpResult._var.toString());
                }
            } else {
                arguments.insert(inIdx, sUNDEF);
                // TODO: handle of parameters with sUNDEF
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

    QVariantMap mp;
    mp["vertex"] = v;
    mp["id"] = _metadata->id();
    emit sendMessage("workflow","currentvertex",mp);

    wait();
    QString argumentlist = arguments.join(",").remove(QRegExp(",+$"));
    QString execString = QString("%1(%2)").arg(meta->name()).arg(argumentlist);
    QString outputs;
    for(int i = 0; i < meta->outputParameterCount(); ++i){
        if ( outputs != "")
            outputs += ",";
        else
            outputs = meta->name();
        outputs += "_" + QString::number(v) + "_" + QString::number(i);
    }
   // execString = outputs + "=" + execString;
    ctx->clear();
    bool success = commandhandler()->execute(execString, ctx, symTable);
    sendData(v,ctx,symTable);

    if ( !success) {
        ERROR1("workflow execution failed when executing: %1", execString);
    } else {
        _nodeExecutionContext[v] = std::make_pair(ctx->_results, symTable);
    }
    return success;

}

bool WorkflowOperationImplementation::checkConditions(const OVertex &v, ExecutionContext *ctx, SymbolTable &symTable)
{
    IWorkflow workflow = (IWorkflow)_metadata;

    QList<ConditionContainer> containers = workflow->getContainersByVertex(v);

    for (int i = 0; i < containers.length(); ++i) {
        if (_containers.contains(i)) {
            // Check for false containers
            if (_containers[i] == false) return false;
        } else {
            // Only if container does has not been executed
            for (Condition condition : containers[i].conditions) {

                // Get all parameter values
                QStringList arguments;
                ExecutionContext localCtx;
                SymbolTable localSymTable;
                for (EdgeProperties edge : condition._edges) {
                    bool ok = reverseFollowExecutionPath(edge._outputOperationIndex, &localCtx, localSymTable);
                    if (!ok) {
                        _containers[i] = false;
                        return false;
                    }
                    QString resultName = localCtx._results[edge._outputParameterIndex];
                    Symbol tmpResult = localSymTable.getSymbol(resultName);
                    arguments.insert(edge._inputParameterIndex, tmpResult._var.toString());

                    copyToContext(tmpResult, resultName, ctx, symTable);
                }

                for (int inputIndex : condition._inputAssignments.keys()) {
                    arguments.insert(inputIndex, condition._inputAssignments[inputIndex].value);
                }

                // Execute condition
                QString execString = QString("%1(%2)").arg(condition._operation->name()).arg(arguments.join(","));
                qDebug() << "Executing condition: " << execString;
                ExecutionContext tempCtx;
                SymbolTable tempSymTable;
                bool success = commandhandler()->execute(execString, &tempCtx, tempSymTable);
                if ( !success) {
                    ERROR1("workflow execution failed when executing condition: %1", execString);
                    _containers[i] = false;
                    return false;
                }

                // Check condition
                // TODO: Only checks for output 0. Maybe try to search for it?
                bool conditionResult = tempSymTable.getSymbol(tempCtx._results[0])._var.value<bool>();
                if (!conditionResult) {  // Condition is not met.
                    _containers[i] = false;
                    return false;
                }
            }

            // If code reaches here, all conditions of the container are met. (Empty containers are also true)
            _containers[i] = true;
        }
    }

    // If code reaches here, all conditions of ALL containers are met.
    return true;
}

