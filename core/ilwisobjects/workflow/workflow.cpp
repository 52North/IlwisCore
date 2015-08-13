#include <array>

#include <QUrl>

#include "kernel.h"
#include "ilwisdata.h"

#include "workflow.h"

using namespace Ilwis;

Workflow::Workflow(): OperationMetaData()
{
}

Workflow::Workflow(const Resource &resource): OperationMetaData(resource)
{
}

Workflow::~Workflow() {

}

SPDataProperties Workflow::addInputDataProperties(const OVertex v)
{
    if ( !_inputProperties.contains(v)) {
        QList<SPDataProperties> list;
        _inputProperties.insert(v, list);
    }
    SPDataProperties properties = SPDataProperties(new DataProperties());
    _inputProperties[v].push_back(properties);
    return SPDataProperties(properties);
}


SPDataProperties Workflow::addOutputDataProperties(const OVertex &v)
{
    if ( !_outputProperties.contains(v)) {
        QList<SPDataProperties> list;
        _outputProperties.insert(v, list);
    }
    SPDataProperties properties = SPDataProperties(new DataProperties());
    _outputProperties[v].push_back(properties);
    return SPDataProperties(properties);
}

void Workflow::removeInputDataProperties(const OVertex &v, quint16 index)
{
    if (_inputProperties.contains(v) && _inputProperties[v].size() > index) {
        _inputProperties[v].removeAt(index);
    }
}

void Workflow::removeOutputDataProperties(const OVertex &v, quint16 index)
{
    if (_outputProperties.contains(v) && _outputProperties[v].size() > index) {
        _outputProperties[v].removeAt(index);
    }
}

OVertex Workflow::addOperation(const NodeProperties &properties)
{
    return boost::add_vertex(properties, _wfGraph);
}

void Workflow::removeOperation(OVertex vertex)
{
    boost::clear_vertex(vertex, _wfGraph);
    boost::remove_vertex(vertex, _wfGraph);
}

NodeProperties Workflow::nodeProperties(const OVertex &v)
{
    return nodeIndex()[v];
}

EdgeProperties Workflow::edgeProperties(const OEdge &e)
{
    return edgeIndex()[e];
}

// nodes where (some) pins do not match an in_edge
QList<OVertex> Workflow::getNodesWithExternalInput()
{
    QList<OVertex> inputNodes;
    boost::graph_traits<WorkflowGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi,vi_end) = boost::vertices(_wfGraph); vi != vi_end; ++vi) {
        OVertex v = *vi;
        quint64 operationId = nodeProperties(v).id;
        if (boost::in_edges(v, _wfGraph).first == boost::in_edges(v, _wfGraph).second) {
            inputNodes.push_back(v); // all pins unassigned
        } else {
            // some pins may be unassigned
            std::vector<quint16> assignedPins = getAssignedPins(v);
            quint16 assignedInputSize = assignedPins.size();

            IOperationMetaData meta = getOperationMetadata(operationId);
            quint16 possibleInputSize = meta->getInputParameters().size();

            if (possibleInputSize > assignedInputSize) {
                inputNodes.push_back(v); // further parameter assignment possible
            }
        }
    }
    return inputNodes;
}

std::vector<quint16> Workflow::getAssignedPins(const OVertex &v)
{
    std::vector<quint16> assignedPins;
    boost::graph_traits<WorkflowGraph>::in_edge_iterator ei, ei_end;
    for (boost::tie(ei,ei_end) = boost::in_edges(v, _wfGraph); ei != ei_end; ++ei) {
        assignedPins.push_back(edgeProperties(*ei).inputIndexNextStep);
    }
    for (SPDataProperties input : _inputProperties[v]) {
        assignedPins.push_back(input->assignedParameterIndex);
    }
    return assignedPins;
}

std::vector<quint16> Workflow::getAssignedPouts(const OVertex &v)
{
    std::vector<quint16> assignedPouts;
    boost::graph_traits<WorkflowGraph>::out_edge_iterator ei, ei_end;
    for (boost::tie(ei,ei_end) = boost::out_edges(v, _wfGraph); ei != ei_end; ++ei) {
        assignedPouts.push_back(edgeProperties(*ei).outputIndexLastStep);
    }
    for (SPDataProperties output : _outputProperties[v]) {
        assignedPouts.push_back(output->assignedParameterIndex);
    }
    return assignedPouts;
}


// nodes where (some) pouts do not match an out_edge
QList<OVertex> Workflow::getNodesWithExternalOutputs()
{
    QList<OVertex> outputNodes;
    qDebug() << "outputs(" << name() << ")=";
    boost::graph_traits<WorkflowGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi,vi_end) = boost::vertices(_wfGraph); vi != vi_end; ++vi) {
        OVertex v = *vi;
        quint64 operationId = nodeProperties(v).id;
        if (boost::out_edges(v, _wfGraph).first == boost::out_edges(v, _wfGraph).second) {
            qDebug() << "out[" << nodeProperties(v).id << "] ";
            outputNodes.push_back(v);
        } else {
            // some pouts may be unassigned
            std::vector<quint16> assignedPouts = getAssignedPouts(v);
            quint16 assignedOutputSize = assignedPouts.size();

            IOperationMetaData meta = getOperationMetadata(operationId);
            quint16 possibleOutputSize = meta->getOutputParameters().size();

            if (possibleOutputSize > assignedOutputSize) {
                outputNodes.push_back(v); // further parameter assignment possible
            }
        }
    }
    return outputNodes;
}

void Workflow::updateNodeProperties(OVertex v, const NodeProperties &properties)
{
    boost::put(nodeIndex(), v, properties);
}

void Workflow::updateEdgeProperties(OEdge e, const EdgeProperties &properties)
{
    boost::put(edgeIndex(), e, properties);
}

OEdge Workflow::addOperationFlow(const OVertex &v1, const OVertex &v2, const EdgeProperties &properties)
{
    return (boost::add_edge(v1, v2, properties, _wfGraph)).first;
}

IlwisTypes Workflow::ilwisType() const
{
    return itWORKFLOW;
}

quint64 Workflow::createMetadata()
{
    parseInputParameters();
    parseOutputParameters();
    return source().id();
}

NodePropertyMap Workflow::nodeIndex()
{
    return get(boost::vertex_index1, _wfGraph);
}

EdgePropertyMap Workflow::edgeIndex()
{
    return get(boost::edge_index, _wfGraph);
}

IOperationMetaData Workflow::getOperationMetadata(quint64 id) const
{
    IOperationMetaData metadata;
    metadata.prepare(mastercatalog()->id2Resource(id));
    if ( !metadata.isValid()) {
        qDebug() << "operation with id" << id << " is unknown!";
        return IOperationMetaData();
    }
    return metadata;
}

void Workflow::parseInputParameters()
{
    qDebug() << "parse workflow input parameters";
    clearInputs();
    int inCount = 0;

    QStringList mandatoryInputs;
    QStringList optionalInputs;
    for (OVertex inputNode : getNodesWithExternalInput()) {
        NodeProperties properties = nodeProperties(inputNode);
        IOperationMetaData meta = getOperationMetadata(properties.id);

        /*
            // TODO parse optional parameters
            // TODO let the user edit parameter names and optionalities
            Resource opResource = rootMeta->source();
            QString inParameters = opResource["inparameters"];
        */
        // add all inparameters for now
        inCount += meta->getInputParameters().size();

        for (SPOperationParameter input : meta->getInputParameters()) {
            SPOperationParameter parameter = newParameter(input.get()); // TODO check copy
            parameter->addToResource(source());
        }
    }
    // TODO replace max number of args with parsed inParameters
    setProperty("inparameters", inCount);

    QString opts = !optionalInputs.isEmpty()
            ? ",[" + optionalInputs.join(",") + "]"
            : "";
    QString workflowSyntax = QString("%1( %2 %3 )").arg(name()).arg(mandatoryInputs.join(",")).arg(opts);
    source().addProperty("syntax", workflowSyntax);

}

void Workflow::parseOutputParameters()
{
    qDebug() << "parse workflow output parameters";
    clearOutputs();
    int outCount = 0;
    for (OVertex outputNode : getNodesWithExternalOutputs()) {
        NodeProperties properties = nodeProperties(outputNode);
        IOperationMetaData outputNodeMeta = getOperationMetadata(properties.id);
        /*
            // TODO parse optional parameters
            // TODO let the user edit parameter names and optionalities
            Resource opResource = rootMeta->source();
            QString outParameters = opResource["outparameters"];
        */
        // add all outparameters for now
        outCount += outputNodeMeta->getOutputParameters().size();

        for (SPOperationParameter output : outputNodeMeta->getOutputParameters()) {
            SPOperationParameter parameter = newParameter(output.get());
            parameter->addToResource(source());
        }
    }
    // TODO replace max number of args with parsed outParameters
    setProperty("outparameters", outCount);
}

void Workflow::debugPrintGraph()
{
    debugPrintVertices();
    debugPrintEdges();
}

void Workflow::debugPrintVertices()
{
    qDebug() << "vertices(g) = ";
    boost::graph_traits<WorkflowGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end) = boost::vertices(_wfGraph); vi != vi_end; ++vi) {
        OVertex v = *vi;
        NodeProperties p = nodeIndex()[v];
        qDebug() << p.id;// /*<< " ";//*/ << " (id=" << v << ") ";
    }
}

void Workflow::debugPrintEdges()
{
    qDebug() << "edges(g) = ";
    boost::graph_traits<WorkflowGraph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(_wfGraph); ei != ei_end; ++ei)
        qDebug() << "(" << nodeIndex()[boost::source(*ei, _wfGraph)].id
                  << "->" << nodeIndex()[boost::target(*ei, _wfGraph)].id
                  //<< "[foo='" << edgeIndex()[*ei].foo << "']"
                  << ") ";
}
