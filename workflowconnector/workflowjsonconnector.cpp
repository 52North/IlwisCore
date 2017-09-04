#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QStringBuilder>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "operationmetadata.h"
#include "operationExpression.h"
#include "operation.h"
#include "jsonconfig.h"
#include "modeller/workflownode.h"
#include "modeller/workflow.h"
#include "WorkflowJSONConnector.h"

using namespace Ilwis;
using namespace WorkflowConnector;

WorkflowJSONConnector::WorkflowJSONConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{

}

bool WorkflowJSONConnector::openTarget() {

    if ( ioOptions().contains("inmemory") && ioOptions()["inmemory"].toBool() == true){
        _data.clear();
        _data.resize(100000);
        QBuffer *buf=  new QBuffer(&_data);
        buf->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
        _datasource.reset( buf );
        return true;
    }else {
        QString filename = _resource.url(true).toLocalFile();
        QFileInfo inf(filename);
        if ( inf.suffix() != "ilwis"){
            filename = inf.absolutePath() + "/" + inf.fileName();
            QString correctUrl = QUrl::fromLocalFile(filename).toString();
            sourceRef().setUrl(correctUrl);
            sourceRef().setUrl(correctUrl,true);
        }
        QFile *file = new QFile(filename);

        if (file->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
            _datasource.reset(file);
            return true;
        }
    }
    return false;
}

void WorkflowJSONConnector::writeWMSLocalLUT(QString filename) {
    QFile *file = new QFile(filename);

    if (file->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
        _datasource.reset(file);
    }
    else return;

    QJsonDocument document;
    QJsonObject links;

    QMapIterator<QString, QString> lut(_layer2LocalLUT);
    while (lut.hasNext()) {
        lut.next();
        links[lut.key()] = lut.value();
    }

    document.setObject(links);
    QTextStream stream(_datasource.get());
    stream << document.toJson();
    stream.device()->close();

}

bool WorkflowJSONConnector::loadMetaData(IlwisObject *object, const IOOptions &)
{
    return false;
}

bool WorkflowJSONConnector::loadData(IlwisObject *, const IOOptions &options)
{
    return false;
}

//bool WorkflowJSONConnector::buildNode(const SPWorkFlowNode& node, const IOOptions &options){
//    _stream << node->name();
//    _stream << node->label();
//    _stream << node->description();
//    _stream << node->id();
//    IOperationMetaData op = node->operation();
//    if ( op.isValid()){
//        QString syntax = op->resource()["syntax"].toString();
//        QString provider = op->resource()["namespace"].toString();
//        _stream << (bool)(op->ilwisType() == itWORKFLOW);
//        _stream << syntax;
//        _stream << provider;
//    }else{
//        _stream << false;
//        _stream << sUNDEF;
//        _stream << sUNDEF;
//    }
//    _stream << (qint32)node->type();
//    _stream << (qint32)node->collapsed();
//    if ( node->type() == WorkFlowNode::ntCONDITION){
//        std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
//        qint32 sz = condition->subnodes("tests").size();
//        _stream << sz;
//        for(qint32 t=0; t < sz; ++t){
//            WorkFlowCondition::Test test = condition->test(t)    ;
//            _stream << (qint32)test._pre;
//            _stream << (qint32)test._post;
//           storeNode(test._operation);
//        }
//        auto operations = condition->subnodes("operations")    ;
//        _stream << (int)operations.size();
//        for(qint32 o=0; o < operations.size(); ++o){
//            storeNode(operations[o]);
//        }
//    }
//    node->box().store(_stream);

//    qint32 count = node->inputCount();
//    if ( node->type() == WorkFlowNode::ntJUNCTION)
//        // junctions have 3 parameters; but inputcount returns only 1 as the link to the condition is the only explicit parameter;
//        //links to operations are implicit. Needed for saving though so we overrule here
//        count = 3;
//    _stream << count;



//    for(qint32 i = 0; i < count; ++i){
//        WorkFlowParameter& wp = node->inputRef(i);
//        _stream << wp.order();
//        _stream << wp.name();
//        _stream << wp.description();
//        _stream << wp.label();
//        _stream << wp.flowLabel();
//        _stream << wp.valueType();
//        _stream << (qint32)wp.state();
//        _stream << wp.attachement(true);
//        _stream << wp.attachement(false);
//        _stream << wp.syntax();
//        auto line = wp.line();
//        _stream << line.size();
//        for(int p=0; p< line.size();++p ){
//            line[i].store(_stream);
//        }
//        if(!VersionedSerializer::store(wp.value(), wp.valueType(), options))
//            return false;
//    }
//    return true;
//}


bool WorkflowJSONConnector::store(IlwisObject *object, const IOOptions &options)
{
    Workflow *workflow = static_cast<Workflow *>(object);
    QJsonDocument document;
    QJsonObject allworkflows;
    QJsonArray workflows;   // top-level object, contains array of workflows

    _layer2LocalLUT.clear();   // clear layer to local file lookup table

    //TODO: resove hack: rawcontainer of the workflow contains the wrong info, so:
    // extra steps to get workflow folder
    Resource wf_res = workflow->resource();
    QString ppp = wf_res.url(true).toLocalFile();
    QFileInfo qfi(ppp);
    QDir par = qfi.dir();
    QString workflowPath = par.absolutePath(); //workflow->resourceRef().container(true).toLocalFile();
    _config.loadSystemConfig(QString("%1/config.json").arg(workflowPath));
    _config.loadUserConfig(QString("%1/userconfig.json").arg(workflowPath));

    QJsonObject single = createJSONWorkflow(workflow->resource());
    single["metadata"] = createJSONWorkflowMetaData(workflow->resource());

    // get all the connections; keep a list of output operations
    single["connections"] = createJSONOperationConnectionList(workflow);

    // deal with all the operations
    QJsonArray operations;

    std::vector<SPWorkFlowNode> outNodes = workflow->outputNodes();
    std::vector<SPWorkFlowNode> nodes = workflow->nodes();
    for (auto node : nodes) {
        if (node->type() == WorkFlowNode::ntOPERATION) {
            QJsonObject operation;

            operation["id"] = (int) node->id();
            operation["metadata"] = createJSONOperationMetadata(node, outNodes);
            operation["inputs"] = createJSONOperationInputList(node);
            operation["outputs"] = createJSONOperationOutputList(node);
            operations.append(operation);
        }

    }



//    QList<OVertex> inputOperations = workflow->getNodesWithExternalInput();
//    parseInputNodeArguments(workflow, inputOperations);

//    std::pair<WorkflowVertexIterator, WorkflowVertexIterator> nodeIterators = workflow->getNodeIterators();

//    for (auto iter = nodeIterators.first; iter < nodeIterators.second; ++iter) {
//        OVertex v = *iter;
//        NodeProperties nodeData = workflow->nodeProperties(*iter);
//        Resource res = mastercatalog()->id2Resource(nodeData._operationid);
//        IOperationMetaData metadata;
//        metadata.prepare(res);
//        if (res.isValid()) {
//            QJsonObject operation;
//            QJsonObject obj = createJSONOperationMetadata(res, v);
//            operation["id"] = (int) v;
//            operation["metadata"] = obj;
//            operation["inputs"] = createJSONOperationInputList(workflow, v);
//            operation["outputs"] = createJSONOperationOutputList(workflow, v);
//            operations.append(operation);
//        }
//    }
    single["operations"] = operations;

    workflows.append(single);

    allworkflows["workflows"] = workflows;
    document.setObject(allworkflows);

    // open output file, and write Json stream to it
    if (!openTarget())
        return false;

    QTextStream stream(_datasource.get());
    stream << document.toJson();
    stream.device()->close();

    // write the LUT (WMS layername <-> local name)
    QFileInfo filename(QDir(workflowPath), "wms2local.json");
    writeWMSLocalLUT(filename.absoluteFilePath());

    return true;
}

//void WorkflowJSONConnector::parseOutputNames(OperationExpression expression)
//{
//    QString expr = expression.input<QString>(0); // we dont want the script command, just its tail
//    int assignIndex = expr.indexOf("=");
//    QString rightTerm = expr.mid(assignIndex + 1);
//    QString leftTerm = expr.left(assignIndex);
//    QStringList outs = leftTerm.split("},");
//    for(auto output : outs){
//        QString res ;
//        int index = output.lastIndexOf("/");
//        if ( index != -1){
//            res = output.mid(index + 2);
//        }else
//            res = output;
//        if (!res.endsWith("}")) // due to the nature how split works the last '}' will have been dropped
//            res += "}";
//        _outputNames.append(res);
//    }
//    _expression = rightTerm;
//}

//void WorkflowJSONConnector::parseInputNodeArguments(Workflow *workflow, const QList<OVertex> &inputNodes)
//{
//    // cache inputs, might go into multiple operations
//    QMap<SPAssignedInputData, QString> inputs;

//    quint16 inputParamIndex = 0;
//    for (OVertex inputOperation : inputNodes) {
//        QStringList requireds;
//        QStringList optionals;
//        IOperationMetaData meta = workflow->getOperationMetadata(inputOperation);
//        meta->parametersFromSyntax(requireds, optionals);
//        QStringList arguments;
//        for (int i = 0 ; i < meta->inputParameterCount() ; i++) {
//            arguments.push_back(""); // initialize empty arguments
//        }

//        // ------------ parse required arguments
//        for (int argIndex = 0 ; argIndex < requireds.size(); argIndex++) {
//            if ( !workflow->hasInputAssignment(inputOperation, argIndex)) {
//                continue; // implicit input
//            } else {
//                SPAssignedInputData inputData = workflow->getAssignedInputData({inputOperation, argIndex});
//                if (inputs.contains(inputData)) {
//                    // shared over multiple operations
//                    arguments.replace(argIndex, inputs.value(inputData));
//                } else {
//                    QString argument;
//                    if (inputData->value.size() > 0) {
//                        argument = inputData->value;    // pre-filled input value
//                    } else {
//                        argument = QString("");
//                        inputParamIndex++;
//                    }
//                    inputs.insert(inputData, argument);
//                    arguments.replace(argIndex, argument);
//                }
//            }
//        }

//        // ------------ parse optional arguments
//        std::vector<SPOperationParameter> parameters = meta->getInputParameters();
//        if (inputParamIndex < _expression.parameterCount()) {
//            for (int i = 0 ; i < optionals.size(); i++) {
//                quint16 optionalIndex = requireds.size() + i;
//                if ( !workflow->hasInputAssignment(inputOperation, optionalIndex)) {
//                    continue; // implicit or non-existent input
//                } else {
//                    SPAssignedInputData inputData = workflow->getAssignedInputData({inputOperation, optionalIndex});
//                    SPOperationParameter parameter = parameters.at(optionalIndex);
//                    QString namedOptional = "%1";/*parameter->term() + "=%1"*/ // TODO named optionals
//                    if (inputs.contains(inputData)) {
//                        // shared over multiple operations
//                        arguments.replace(optionalIndex, namedOptional.arg(inputs.value(inputData)));
//                    } else {
//                        QString argument;
//                        if (inputData->value.size() > 0) {
//                            argument = inputData->value;    // pre-filled input value
//                        } else {
//                            argument = _expression.parm(inputParamIndex).value();
//                            inputParamIndex++;
//                        }
//                        inputs.insert(inputData, argument);
//                        arguments.replace(optionalIndex, namedOptional.arg(argument));
//                    }
//                }
//            }
//        }

//        // ------------ cache to be available when node gets executed

//        _inputArgs.insert(inputOperation, arguments);
//    }
//}

//void WorkflowJSONConnector::addGeneratedNames(const OVertex &v, QStringList& names, const IOperationMetaData& meta)
//{
//    int n = meta->outputParameterCount();
//    for(int i=0; i < n; ++i){
//        QString name = meta->name() + "_" + QString::number(v) + "_" + QString::number(i);
//        names.append(name);
//    }
//    _nodeExecutionContext[v] = names;
//}

QString WorkflowJSONConnector::type() const
{
    return "workflow";
}


bool WorkflowJSONConnector::isReadOnly() const
{
    return false;
}

QString WorkflowJSONConnector::provider() const
{
    return "Json";
}

QVariant WorkflowJSONConnector::getProperty(const QString &) const
{
    return QString("");
}

Ilwis::IlwisObject *WorkflowJSONConnector::create() const
{
    return new Workflow(_resource);
}

ConnectorInterface *WorkflowJSONConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions &options)
{
    return new WorkflowJSONConnector(resource, load, options);
}

QJsonObject WorkflowJSONConnector::createJSONWorkflow(const Resource &res)
{
    QJsonObject workflow;

    workflow["id"] = 0;
    return workflow;
}


QJsonObject WorkflowJSONConnector::createJSONWorkflowMetaData(const Resource& res){
    QJsonObject meta;
    meta.insert("longname", res.name());
    meta.insert("description", res.description());
    meta.insert("syntax", res["syntax"].toString());
    meta.insert("resource", QString("Ilwis"));
//    meta.insert("keywords", res["keyword"].toString());   // june 2017: there is no way to set workflows keywords
    meta.insert("inputparametercount", res["inparameters"].toInt());
    meta.insert("outputparametercount", res["outparameters"].toInt());

    return meta;
}

QJsonObject WorkflowJSONConnector::createJSONOperationMetadata(const SPWorkFlowNode& node, const std::vector<SPWorkFlowNode>& outNodes) {
    QJsonObject jsonMeta;
    jsonMeta["longname"] = node->name();
    jsonMeta["label"] = node->label();
    jsonMeta["description"] = node->description();

    quint64 nid = node->id();
    std::vector<SPWorkFlowNode>::const_iterator p = std::find_if(
                outNodes.begin(),
                outNodes.end(),
                [nid] (const SPWorkFlowNode& nod) { return nod->id() == nid; }
    );
    if (p != outNodes.end())
        jsonMeta["final"] = QString("true");
    else
        jsonMeta["final"] = QString("false");

    IOperationMetaData op = node->operation();
    if (op.isValid()){
        QString provider = op->resource()["namespace"].toString();
        jsonMeta["resource"] = provider;
        jsonMeta["syntax"] = op->resource()["syntax"].toString();
        QString keywords = op->resource()["keyword"].toString();
        jsonMeta["keywords"] = keywords;
    }

    jsonMeta["inputparametercount"] = node->inputCount();
    jsonMeta["outputparametercount"] = (int) op->outputParameterCount();

    return jsonMeta;
}

//void WorkflowJSONConnector::setInputParm(const QString baseName, const SPOperationParameter parm, QJsonObject& input, int nameType)
//{
//    if (baseName.length() == 0)
//        return;

//    if (parm) {
//        if (hasType(parm->type(), itCOVERAGE)) {
//            QString localName = baseName;
//            QFileInfo fi(baseName);

//            QUrl url(baseName);
//            QString scheme = url.scheme();
//            if (scheme.length() == 0)
//                localName = _config.getLocalName(fi.baseName());
//            else if (url.isLocalFile())
//                localName = url.toLocalFile();

//            fi.setFile(localName);
//            input["local"] = localName;   // note: can be local or regular url
//            QString layerName;
//            QString wms = _config.getWMSGetMapURL(fi.baseName(), layerName);
//            input["url"] = wms;
//            // for now generated internal coverages are available in WMS, but are switched off for showing
//            input["show"] = nameType == ASSIGNED_INPUT;

//            if (localName.length() > 0)
//                _layer2LocalLUT[layerName] = localName;
//        }
//        else {
//            QString local = baseName;
//            QUrl url(baseName);
//            if (url.isLocalFile())
//                local = url.toLocalFile();
//            input["value"] = local;
//        }
//    }
//}

/*
    Add all workflow input fields for an operation to the json document. This happens both
    for internal connections and actual input nodes.

    Pre-assigned inputs (fixed values) are taken as is (it is assumed they point to actual existing objects)

    External inputs that are not specified remain empty (this should be avoided, because that
    invalidates the output Json file!).

    Internal inputs get a name (auto-generated) based on the name of the operation
    that provides the input and its corresponding output edge number. The user input folder
    is used to create an actual local file location.

    Every input is also added into a local file list. The list links the WMS layername
    with the local filename. This is necessary to find the correct local file, when the
    Json file is interpreted as a workflow.
 */
QJsonArray WorkflowJSONConnector::createJSONOperationInputList(const SPWorkFlowNode& node) {
    QJsonArray inputs;

    for (int i = 0; i < node->inputCount(); i++) {
        QJsonObject input;

        WorkFlowParameter& wfp = node->inputRef(i);
        input["url"] = QString("");
        input["local"] = QString("");
        input["value"] = QString("");
        input["name"] = wfp.label();
        input["id"] = wfp.order();
        input["description"] = wfp.description();

        input["change"] = QString("false");
        input["show"] = QString("false");
        input["type"] = QString("");
        if (wfp.state() == WorkFlowParameter::pkFIXED)
            input["value"] = wfp.value();

        IOperationMetaData op = node->operation();
        if (op.isValid()) {
            SPOperationParameter parm = op->getInputParameters()[i];
            input["optional"] = parm->isOptional();
            bool isExp = hasType(parm->type(), itSTRING) && wfp.value().contains('@');
            bool isNum = hasType(parm->type(), itNUMBER);
            bool isMap = hasType(parm->type(), itRASTER );    // or should this be itCOVERAGE?
            if (isNum || isMap) input["show"] = QString("true");
            if (isNum) input["type"] = QString("number");
            if (isMap) input["type"] = QString("map");
            if (isExp) input["type"] = QString("expression");
        }
        inputs.append(input);
    }
//    // get the metadata from the operation
//    IOperationMetaData meta = workflow->getOperationMetadata(v);
//    std::vector<SPOperationParameter> params = meta->getInputParameters();
//    foreach (SPOperationParameter parameter, params) {
//        QJsonObject input;
//        input["url"] = QString("");
//        input["local"] = QString("");
//        input["value"] = QString("");
//        input["name"] = parameter->term();  // weird naming!!
//        input["type"] = parameter->name();  // weird naming!!
//        input["optional"] = parameter->isOptional();
//        input["description"] = parameter->description();

//        inputs.append(input);
//    }

//    // external inputs; this may include pre-assigned inputs
//    QStringList externalInputs = _inputArgs.value(v);
//    for (int i = 0 ; i < externalInputs.size() ; i++) {
//        auto input = inputs[i].toObject();
//        input["id"] = i;
//        QString externalInput = externalInputs.at(i);
//        QString localName = "";
//        if ( !externalInput.isEmpty()) {
//            int parmIndex = externalInput.indexOf("?input_");
//            if (parmIndex == -1)
//                localName = externalInput;
//        }

//        SPOperationParameter parm = meta->inputParameter(i);
//        setInputParm(localName, parm, input, EXTERNAL_INPUT);

//        inputs.replace(i, input);
//    }

//    // pre-assigned values, fully qualified filename / url
//    for (InputAssignment assignment : workflow->getConstantInputAssignments(v)) {
//        SPAssignedInputData inputValue = workflow->getAssignedInputData(assignment);
//        QJsonObject input = inputs.at(assignment.second).toObject();
//        input["id"] = assignment.second;

//        SPOperationParameter parm = meta->inputParameter(assignment.second);
//        setInputParm(inputValue->value, parm, input, ASSIGNED_INPUT);

//        inputs.replace(assignment.second, input);
//    }

//    // internal calculation result
//    InEdgeIterator ei, ei_end;
//    boost::tie(ei, ei_end) = workflow->getInEdges(v);
//    for (; ei != ei_end; ei++) {
//        OVertex previous = workflow->getSourceOperationNode(*ei);
//        IOperationMetaData metaPrev = workflow->getOperationMetadata(previous);
//        QStringList names;
//        addGeneratedNames(previous, names, metaPrev);

//        EdgeProperties edges = workflow->edgeProperties(*ei);
//        quint16 inputEdge = edges._inputParameterIndex;
//        quint16 outputEdge = edges._outputParameterIndex;
//        auto input = inputs[inputEdge].toObject();
//        input["id"] = inputEdge;

//        SPOperationParameter parm = metaPrev->outputParameter(outputEdge);
//        setInputParm(names[outputEdge], parm, input, GENERATED_INPUT);

//        inputs.replace(inputEdge, input);
//    }

    return inputs;
}

/*
    Add all workflow output fields for an operation to the json document. This happens both
    for internal connections and actual output nodes.


 */
QJsonArray WorkflowJSONConnector::createJSONOperationOutputList(const SPWorkFlowNode& node) {
    QJsonArray outputs;

    IOperationMetaData op = node->operation();
    if (op.isValid()) {

        int index = 0;
        std::vector<SPOperationParameter> params = op->getOutputParameters();
        foreach (SPOperationParameter opParam, params) {
            QJsonObject output;
            QString baseName("");
            output["local"] = baseName;
            output["url"] = baseName;
            output["value"] = baseName;
            output["name"] = opParam->term();  // weird naming!!
            output["type"] = opParam->name();  // weird naming!!
            output["id"] = index;
            output["optional"] = opParam->isOptional();
            output["description"] = opParam->description();

            bool isNum = hasType(opParam->type(), itNUMBER);
            bool isMap = hasType(opParam->type(), itRASTER );    // or should this be itCOVERAGE?
            if (isNum || isMap) output["show"] = QString("true");
            if (isNum) output["type"] = QString("number");
            if (isMap) output["type"] = QString("map");

//            if (actual[index] != -1) { // an actual workflow output parameter
//                names[index] += "_out";
//            }
            index++;

            outputs.append(output);
        }
    }


//    // get the metadata from the operation
//    IOperationMetaData meta = workflow->getOperationMetadata(v);
//    std::vector<SPOperationParameter> params = meta->getOutputParameters();
//    QStringList names;
//    addGeneratedNames(v, names, meta);

//    // Identify the parameters that are actual workflow outputs
//    std::vector<int> actual(params.size());
//    int n = 0;
//    std::generate(actual.begin(), actual.end(), [&n]{ return n++; });
//    std::pair<OutEdgeIterator, OutEdgeIterator> ei = workflow->getOutEdges(v);
//    for (auto &edge = ei.first; edge < ei.second; ++edge) {
//        EdgeProperties edgeData = workflow->edgeProperties(*edge);
//        actual[edgeData._outputParameterIndex] = -1;    // has internal connection, so remove from actual outputs
//    }

//    int index = 0;
//    foreach (SPOperationParameter parameter, params) {
//        QJsonObject output;
//        QString baseName("");
//        output["local"] = baseName;
//        output["url"] = baseName;
//        output["value"] = baseName;
//        output["name"] = parameter->term();  // weird naming!!
//        output["type"] = parameter->name();  // weird naming!!
//        output["id"] = index;
//        output["optional"] = parameter->isOptional();
//        output["description"] = parameter->description();
//        if (actual[index] != -1) { // an actual workflow output parameter
//            names[index] += "_out";
//        }
//        baseName = names[index];
//        QString localName = _config.getLocalName(baseName);
//        if (hasType(parameter->type(), itCOVERAGE)) {
//            output["show"] = true;
//            output["local"] = localName;
//            QString layerName;
//            QString wms = _config.getWMSGetMapURL(localName, layerName);
//            output["url"] = wms;
//            _layer2LocalLUT[layerName] = localName;
//        }
//        else if (hasType(parameter->type(), itTABLE))
//            output["value"] = baseName;
//        else {
//            output["value"] = localName;
//        }
//        index++;

//        outputs.append(output);
//    }

    return outputs;
}

QJsonArray WorkflowJSONConnector::createJSONOperationConnectionList(Workflow *workflow) {
    QJsonArray connections;

    _leafOperations.clear();
    QSet<int> connectedOperations;

    std::vector<SPWorkFlowNode> nodes = workflow->nodes();
    for (auto node : nodes) {
        QJsonObject connection;

        if (node->type() == WorkFlowNode::ntOPERATION) {
            for (int i = 0; i < node->inputCount(); ++i) {
                if (node->inputRef(i).inputLink()) {
                    WorkFlowParameter wfp = node->inputRef(i);
                    int toParamID = wfp.order();
                    int toOperID = wfp.nodeId();
                    int fromParamID = wfp.outputParameterIndex();
                    int fromOperID = wfp.inputLink()->id();

                    connection["fromOperationID"] = fromOperID;
                    connection["fromParameterID"] = fromParamID;
                    connection["toOperationID"] = toOperID;
                    connection["toParameterID"] = toParamID;

                    connections.append(connection);
                }
            }
        }

    }

//    std::pair<WorkflowVertexIterator, WorkflowVertexIterator> nodeIterators = workflow->getNodeIterators();
//    for (auto iterOper = nodeIterators.first; iterOper < nodeIterators.second; ++iterOper) {
//        std::pair<OutEdgeIterator, OutEdgeIterator> edgeIterators = workflow->getOutEdges(*iterOper);

//        for (auto &edge = edgeIterators.first; edge < edgeIterators.second; ++edge) {
//            OVertex toVertex = workflow->getTargetOperationNode(*edge);
//            EdgeProperties edgeData = workflow->edgeProperties(*edge);
//            QJsonObject connection;
//            connection["fromOperationID"] = (int) *iterOper;
//            connection["fromParameterID"] = edgeData._outputParameterIndex;
//            connection["toOperationID"] = (int) toVertex;
//            connection["toParameterID"] = edgeData._inputParameterIndex;

//            connections.append(connection);

//            connectedOperations.insert((int) *iterOper);    // operations with input connections from operations
//            _leafOperations.insert((int) toVertex);         // operations with output connections to other operations
//        }
//    }
//    _leafOperations.subtract(connectedOperations);  // this leaves only output operations that are not connected

    return connections;
}
