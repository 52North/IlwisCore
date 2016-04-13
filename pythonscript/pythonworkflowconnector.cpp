#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "operationmetadata.h"
#include "workflow.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "pythonworkflowconnector.h"

using namespace Ilwis;
using namespace PythonScript;

PythonWorkflowConnector::PythonWorkflowConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) :
    IlwisObjectConnector(resource, load, options)
{

}

bool PythonWorkflowConnector::openTarget() {
    QString filename = _resource.url(true).toLocalFile();
    QFileInfo inf(filename);
    if ( inf.suffix() != "ilwis"){
        filename = inf.absolutePath() + "/" + inf.fileName();
        QString correctUrl = QUrl::fromLocalFile(filename).toString();
        source().setUrl(correctUrl);
        source().setUrl(correctUrl,true);
    }
    QFile *file = new QFile(filename);

    if (file->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
        _datasource.reset(file);
        return true;
    }
    return false;
}

bool PythonWorkflowConnector::store(IlwisObject *obj, const IOOptions &options)
{
    Workflow *workflow = static_cast<Workflow *>(obj);

    //bool inMemory = true;
    if ( !options.contains("expression"))
        return false;

    auto expression = options["expression"].value<OperationExpression>();
    QString expr = expression.input<QString>(0); // we dont want the script command, just its tail
    int assignIndex = expr.indexOf("=");
    QString rightTerm = expr.mid(assignIndex + 1);
    QString leftTerm = expr.left(assignIndex);
    QStringList outs = leftTerm.split("},");
    for(auto output : outs){
        QString res ;
        int index = output.lastIndexOf("/");
        if ( index != -1){
            res = output.mid(index + 2);
        }else
            res = output;
        if (!res.endsWith("}")) // due to the nature how split works the last '}' will have been dropped
            res += "}";
        _outputNames.append(res);
    }
    _expression = rightTerm;

    QStringList names, script;
    QList<OVertex> inputNodes = workflow->getNodesWithExternalInput();
    parseInputNodeArguments(workflow, inputNodes);

    QList<OVertex> outputNodes = workflow->getNodesWithExternalOutputs();
    script.append("import ilwisobjects as ilwis");
    for (int i = 0; i<outputNodes.size(); ++i) {
        bool ok = reverseFollowScriptPath(workflow, outputNodes[i], names, script,i);
        if (!ok) {
            ERROR0("workflow deduction failed when trying to generate script!");
            return false;
        }

   }

    if (!openTarget())
        return false;
    QTextStream stream(_datasource.get());
    QString txt = script.join("\n");
    int parmIndex;
    int count = 1;
    while((parmIndex = txt.indexOf("?input_")) != -1){
        QString front = txt.left(parmIndex);
        QString back = txt.mid(parmIndex + 8);
        txt = front + "argv[" + QString::number(count) + "]" + back;
        ++count;
    }
    while((parmIndex = txt.indexOf("?output_")) != -1){
        QString front = txt.left(parmIndex);
        QString back = txt.mid(parmIndex + 9);
        txt = front + "argv[" + QString::number(count) + "]" + back;
        ++count;
    }
    stream << txt;
    stream.device()->close();
    return true;

}

void PythonWorkflowConnector::parseInputNodeArguments(Workflow *workflow, const QList<OVertex> &inputNodes)
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
                        argument =  _expression.parm(inputParamIndex).value();
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

void PythonWorkflowConnector::executeInputNode(Workflow *workflow,const OVertex &v, QStringList& names, QStringList& script)
{
    IOperationMetaData meta = workflow->getOperationMetadata(v);

    QString execString = QString("ilwis.Engine.do(\"%1\",%2)");
    execString = execString.arg(meta->name());
    QString args = createArgumentList(meta, _inputArgs[v]);
    execString = execString.arg(args);
    addGeneratedNames(v,names,meta);
    QString expr = names[0] + "=" + execString;
    script.append(expr);
}
void PythonWorkflowConnector::addGeneratedNames(const OVertex &v, QStringList& names, const IOperationMetaData& meta)
{
    int n = meta->outputParameterCount();
    for(int i=0; i < n; ++i){
        QString name = meta->name() + "_" + QString::number(v) + "_" + QString::number(i);
        names.append(name);
    }
    _nodeExecutionContext[v] = names;
}

bool PythonWorkflowConnector::reverseFollowScriptPath(Workflow *workflow, const OVertex &v, QStringList& names, QStringList& script, int order){
    if (_nodeExecutionContext.contains(v)) {
        names = _nodeExecutionContext[v];
        return true;
    }
    IOperationMetaData meta = workflow->getOperationMetadata(v);

    InEdgeIterator ei, ei_end;
    boost::tie(ei,ei_end) = workflow->getInEdges(v);

    if( doCondition(meta, v,names))
        return true;
    // has external inputs only
    if (ei == ei_end/*workflow->isInputNode(v)*/) {
        executeInputNode(workflow, v, names, script);
    }

    QStringList arguments;

    for (InputAssignment assignment : workflow->getConstantInputAssignments(v)) {
        SPAssignedInputData input = workflow->getAssignedInputData(assignment);
        arguments.insert(assignment.second, input->value);
    }

    if(!doInputEdges(ei, ei_end,workflow,v,arguments, script))
        return false;

    QStringList externalInputs = _inputArgs.value(v);
    for (int i = 0 ; i < externalInputs.size() ; i++) {
        QString externalInput = externalInputs.at(i);
        if ( !externalInput.isEmpty()) {
            arguments.insert(i, externalInput);
        }
    }
    if (! _nodeExecutionContext.contains(v)){

        QString args = createArgumentList(meta, arguments);
        //QString argumentlist = arguments.join(",").remove(QRegExp(",+$"));
        QString execString = QString("ilwis.Engine.do(\"%1\",%2)").arg(meta->name()).arg(args);;
        addGeneratedNames(v, names, meta);
        QString storeStatement;
        QString out = order == -1 ? names[0] : _outputNames[order]; // order is only used for external (user) set names while else we used generated names
        int index = out.indexOf("{");
        if ( index != -1){
            QString format = out.mid(index + 8,out.size() - 10 - index);
            QString outName = out.left(index);
            out = names[order == -1 ? 0 : order];

            storeStatement = QString("%1.store(%2,\"%3\",\"%4\")").arg(out).arg(outName).arg(format.split(",")[1]).arg(format.split(",")[0]);
        }
        QString expr = out + "=" + execString;

        script.append(expr);
        if ( storeStatement != "")
            script.append(storeStatement);

    }
    return true;
}

QString PythonWorkflowConnector::createArgumentList(const IOperationMetaData& meta, const QStringList& arguments){
    QString args;
    QString argumentlist = arguments.join(",").remove(QRegExp(",+$"));
    QStringList parts = argumentlist.split(",");
    for(int i =0; i < parts.size(); ++i){
        SPOperationParameter parm = meta->inputParameter(i);
        if ( parm){
            if ( args != "")
                args += ",";
            QString value = parts[i];

            if ( hasType(parm->type(), itILWISOBJECT)){
                if ( value.indexOf(":") != -1){ // it is not a generated name but an url
                    args += "\"" +value + "\"" ;
                }else
                    args += value;
            }else {
                if (hasType(parm->type(), itNUMBER)){
                    args += value;
                }else{
                    args += "\"" + value + "\"" ;
                }
            }
        }

    }
    return args;
}

bool PythonWorkflowConnector::doCondition(const IOperationMetaData& meta,const OVertex &v, QStringList &names){
    //TODO later
//    bool conditionsResult = checkConditions(v, ctx, symTable);

//    // IF conditions are not met
//    if (!conditionsResult) {
//        // Return empty outputs and DON'T execute this operation.
//        QString resultName = CONDITION_FAILED;
//        for (int i = 0; i < meta->getOutputParameters().size(); ++i) {
//            ExecutionContext localCtx;
//            SymbolTable localSymTable;
//            localCtx.addOutput(localSymTable, QVariant(), resultName, itSTRING, Resource());
//            copyToContext(localSymTable.getSymbol(localCtx._results[i]), resultName, ctx, symTable);
//        }
//        return true; // Not false because it is valid for conditions to fail.
//    }
    return false;
}

bool PythonWorkflowConnector::doInputEdges(InEdgeIterator& ei,
                                                   const InEdgeIterator& ei_end,
                                                   Workflow *workflow,
                                                   const OVertex &v,
                                                   QStringList& arguments,
                                           QStringList& script) {
    IOperationMetaData meta = workflow->getOperationMetadata(v);
    // for all input parameters that come from previous operations
    for (/*boost::tie(ei,ei_end) = workflow->getInEdges(v)*/; ei != ei_end; ++ei) {
        //get the operation that produces this parameters and see if it can be calculated
        QStringList localNames;
        OVertex previous = workflow->getSourceOperationNode(*ei);
        bool ok = reverseFollowScriptPath(workflow, previous, localNames,script);
        if ( !ok) {
            return false;
        } else {
            EdgeProperties edgeProperties = workflow->edgeProperties(*ei);

            quint16 inIdx = edgeProperties._inputParameterIndex;
            quint16 outIdx = edgeProperties._outputParameterIndex;
            QString resultName = localNames[outIdx];
            arguments.insert(inIdx, resultName);

            if ( !edgeProperties.temporary) {
                QString outputName = edgeProperties.outputName.isEmpty()
                        ? QString("%1_node%2_pout%3").arg(workflow->name()).arg(v).arg(outIdx)
                        : edgeProperties.outputName;

                    // make unique in shared execution context/symbol table
                    outputName = QString("%1_node%2_pout%3_%4").arg(workflow->name()).arg(v).arg(outIdx).arg(outputName);
               // copyToContext(symbol, outputName, ctx, symTable);
            }
        }
    }
    return true;
}

QString PythonWorkflowConnector::type() const
{
    return "Workflow";
}

IlwisObject *PythonWorkflowConnector::create() const
{
    return new Workflow(_resource  );
}

QString PythonWorkflowConnector::provider() const
{
    return "python";
}

ConnectorInterface *PythonWorkflowConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions &options)
{
   return new PythonWorkflowConnector(resource, load, options);
}
