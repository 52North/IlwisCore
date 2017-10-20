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
#include "workflownode.h"
#include "junctionNode.h"
#include "operationnode.h"
#include "conditionNode.h"
#include "rangenode.h"
#include "rangejunctionnode.h"
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

bool WorkflowJSONConnector::loadMetaData(IlwisObject *object, const IOOptions &)
{
    return false;
}

bool WorkflowJSONConnector::loadData(IlwisObject *, const IOOptions &options)
{
    return false;
}

bool WorkflowJSONConnector::store(IlwisObject *object, const IOOptions &options)
{
    Workflow *workflow = static_cast<Workflow *>(object);
    QJsonDocument document;
    QJsonObject allworkflows;
    QJsonArray workflows;   // top-level object, contains array of workflows

//    _layer2LocalLUT.clear();   // clear layer to local file lookup table

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

    // deal with all the operations
    QJsonArray operations;

    std::vector<SPWorkFlowNode> outNodes = workflow->outputNodes();
    std::vector<SPWorkFlowNode> nodes = workflow->nodes();
    // Json operation ID's are expected to start from zero, however workflow operations ID's
    // can have any positive value, and do not necessarily start at zero.
    // A Json ID is assigned to each workflow operation.
    // We keep a record of the actual operation ID's and the assigned Json ID's
    std::map<quint64, int> nodeMapping;
    int jsonOperID = 0;
    int jsonOtherID = std::count_if(nodes.begin(), nodes.end(), [](SPWorkFlowNode node) { return node->type() == WorkFlowNode::ntOPERATION;});
    for (auto node : nodes) {
        QJsonObject operation;
        if (node->type() == WorkFlowNode::ntOPERATION) {
            operation["id"] = jsonOperID;
            nodeMapping[node->id()] = jsonOperID++;  // keep track of the Json ID as function of the internal operation ID

            operation["metadata"] = createJSONOperationMetadata(node, outNodes);
            operation["inputs"] = createJSONOperationInputList(node);
            operation["outputs"] = createJSONOperationOutputList(node);
        }
        else if (node->type() == WorkFlowNode::ntRANGE) {
            operation["id"] = jsonOtherID;
            nodeMapping[node->id()] = jsonOtherID++;
            operation["metadata"] = createJSONRangeMetadata(node);
        }
        else if (node->type() == WorkFlowNode::ntJUNCTION) {
            operation["id"] = jsonOtherID;
            nodeMapping[node->id()] = jsonOtherID++;

        }
        else if (node->type() == WorkFlowNode::ntRANGEJUNCTION) {
            operation["id"] = jsonOtherID;
            nodeMapping[node->id()] = jsonOtherID++;

        }
        else {
            operation["id"] = jsonOtherID;
            nodeMapping[node->id()] = jsonOtherID++;
            operation["metadata"] = createJSONOperationMetadata(node, outNodes);
        }
        operations.append(operation);
    }

    // get all the connections; keep a list of output operations;
    // take care of remapping to the Json operation ID's
    single["connections"] = createJSONOperationConnectionList(workflow, nodeMapping);

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

    return true;
}

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

QJsonObject WorkflowJSONConnector::createJSONRangeMetadata(const SPWorkFlowNode& node) {
    QJsonObject jsonMeta;
    jsonMeta["longname"] = node->name();
    jsonMeta["label"] = node->label();
    jsonMeta["description"] = node->description();

    jsonMeta["final"] = false;

    jsonMeta["inputparametercount"] = node->inputCount();

    std::shared_ptr<RangeNode> range = std::static_pointer_cast<RangeNode>(node);
    QString rangedef =  range->rangeDefinition( );

    jsonMeta["range"] = rangedef;

    return jsonMeta;
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
        jsonMeta["final"] = true;
    else
        jsonMeta["final"] = false;

    IOperationMetaData op = node->operation();
    if (op.isValid()){
        QString provider = op->resource()["namespace"].toString();
        jsonMeta["resource"] = provider;
        jsonMeta["syntax"] = op->resource()["syntax"].toString();
        QString keywords = op->resource()["keyword"].toString();
        jsonMeta["keywords"] = keywords;

        jsonMeta["outputparametercount"] = (int) op->outputParameterCount();

    }

    jsonMeta["inputparametercount"] = node->inputCount();

    return jsonMeta;
}

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

        input["change"] = false;
        input["show"] = false;
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
            if (isNum || isMap) input["show"] = true;
            if (isNum) input["type"] = QString("number");
            if (isMap) input["type"] = QString("map");
            if (isExp) input["type"] = QString("expression");
        }
        inputs.append(input);
    }

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
            output["name"] = opParam->name();
            output["id"] = index;
            output["optional"] = opParam->isOptional();
            output["description"] = opParam->description();

            bool isNum = hasType(opParam->type(), itNUMBER);
            bool isMap = hasType(opParam->type(), itRASTER );    // or should this be itCOVERAGE?
            if (isNum || isMap) output["show"] = true;
            if (isNum) output["type"] = QString("number");
            if (isMap) output["type"] = QString("map");

            index++;

            outputs.append(output);
        }
    }

    return outputs;
}

QJsonArray WorkflowJSONConnector::createJSONOperationConnectionList(Workflow *workflow, std::map<quint64, int> nodeMapping) {
    QJsonArray connections;

    _leafOperations.clear();

    std::vector<SPWorkFlowNode> nodes = workflow->nodes();
    for (auto node : nodes) {
        QJsonObject connection;

//        if (node->type() == WorkFlowNode::ntOPERATION) {
        int nrInputs = node->inputCount();

        // junctions have 3 or 4 parameters; but inputcount returns only 1 as the link to the condition is the only explicit parameter;
        // links to operations are implicit. Needed for saving though so we overrule here; see also WorkflowSerializerV1::storeNodeLinks()
        if (node->type() == WorkFlowNode::ntJUNCTION)
            nrInputs = 3;
        if (node->type() == WorkFlowNode::ntRANGEJUNCTION)
            nrInputs = 4;

        for (int i = 0; i < nrInputs; ++i) {
            if (node->inputRef(i).inputLink()) {
                WorkFlowParameter wfp = node->inputRef(i);
                int toParamID = wfp.order();
                int toOperID = nodeMapping[wfp.nodeId()];
                int fromParamID = wfp.outputParameterIndex();
                int fromOperID = nodeMapping[wfp.inputLink()->id()];

                connection["fromOperationID"] = fromOperID;
                connection["fromParameterID"] = fromParamID;
                connection["toOperationID"] = toOperID;
                connection["toParameterID"] = toParamID;

                connections.append(connection);
            }
        }

    }

    return connections;
}
