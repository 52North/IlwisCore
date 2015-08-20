#include <algorithm>
#include <array>

#include <QUrl>

#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "operationExpression.h"
#include "operation.h"

#include "workflow.h"
#include "workflowoperationimplementation.h"

using namespace Ilwis;

Workflow::Workflow(): OperationMetaData()
{
}

Workflow::Workflow(const Resource &resource): OperationMetaData(resource)
{
}

Workflow::~Workflow() {

}

SPInputDataProperties Workflow::addInputDataProperties(const OVertex &v, quint16 index)
{
    SPInputDataProperties properties = SPInputDataProperties(new InputDataProperties());
    assignInputData(v, properties, index);
    return properties;
}

void Workflow::assignInputData(const OVertex &v, const SPInputDataProperties &properties, quint16 index)
{
    if ( !_inputProperties.contains(v)) {
        QList<SPInputDataProperties> list;
        _inputProperties.insert(v, list);
    }
    _inputProperties[v].push_back(properties);

    if ( !_assignedParameterIndexes.contains(properties)) {
        QList<InputAssignment> list;
        _assignedParameterIndexes.insert(properties, list);
    }
    _assignedParameterIndexes[properties].push_back(std::make_pair(v, index));
}

quint16 Workflow::getInputDataAssignment(const SPInputDataProperties &properties, const OVertex &v)
{
    if (_assignedParameterIndexes.contains(properties)) {
        QList<InputAssignment> list = _assignedParameterIndexes[properties];
        auto iter = std::find_if(list.begin(), list.end(), [v](InputAssignment &ia) {
            return ia.first == v;
        });
        if (iter != list.end()) {
            return (*iter).second;
        } else {
            qDebug() << "no input assignment exist for node " << v;
            return -1;
        }
    }
}


SPOutputDataProperties Workflow::addOutputDataProperties(const OVertex &v)
{
    if ( !_outputProperties.contains(v)) {
        QList<SPOutputDataProperties> list;
        _outputProperties.insert(v, list);
    }
    SPOutputDataProperties properties = SPOutputDataProperties(new OutputDataProperties());
    _outputProperties[v].push_back(properties);
    return properties;
}

QList<SPInputDataProperties> Workflow::getInputDataProperties(const OVertex &v) const
{
    if (_inputProperties.contains(v)) {
        return _inputProperties[v];
    } else {
        QList<SPInputDataProperties> emptyList;
        return emptyList;
    }
}

QList<SPOutputDataProperties> Workflow::getOutputDataProperties(const OVertex &v) const
{
    if (_outputProperties.contains(v)) {
        return _outputProperties[v];
    } else {
        QList<SPOutputDataProperties> emptyList;
        return emptyList;
    }
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
    if (_inputNodes.isEmpty()) {
        boost::graph_traits<WorkflowGraph>::vertex_iterator vi, vi_end;
        for (boost::tie(vi,vi_end) = boost::vertices(_wfGraph); vi != vi_end; ++vi) {
            OVertex v = *vi;
            if (boost::in_edges(v, _wfGraph).first == boost::in_edges(v, _wfGraph).second) {
                _inputNodes.push_back(v); // all pins unassigned
            } else {
                // some pins may be unassigned
                std::vector<quint16> assignedPins = getAssignedPins(v);
                quint16 assignedInputSize = assignedPins.size();

                IOperationMetaData meta = getOperationMetadata(v);
                quint16 possibleInputSize = meta->getInputParameters().size();

                if (assignedInputSize < possibleInputSize) {
                    _inputNodes.push_back(v); // further parameter assignment possible
                }
            }
        }
    }
    return _inputNodes;
}

std::vector<quint16> Workflow::getAssignedPins(const OVertex &v)
{
    std::vector<quint16> assignedPins;
    boost::graph_traits<WorkflowGraph>::in_edge_iterator ei, ei_end;
    for (boost::tie(ei,ei_end) = getInEdges(v); ei != ei_end; ++ei) {
        // implicitly assigned pins via edges
        assignedPins.push_back(edgeProperties(*ei).inputIndexNextOperation);
    }
    for (SPInputDataProperties input : _inputProperties[v]) {
        if (input->value.isValid()) {
            // explicitly assigned pins
            assignedPins.push_back(getInputDataAssignment(input, v));
        }
    }
    return assignedPins;
}

std::pair<InEdgeIterator,InEdgeIterator> Workflow::getInEdges(const OVertex &v)
{
    return boost::in_edges(v, _wfGraph);
}

// nodes where (some) pouts do not match an out_edge
QList<OVertex> Workflow::getNodesWithExternalOutputs()
{
    if (_outputNodes.isEmpty()) {
        boost::graph_traits<WorkflowGraph>::vertex_iterator vi, vi_end;
        for (boost::tie(vi,vi_end) = boost::vertices(_wfGraph); vi != vi_end; ++vi) {
            OVertex v = *vi;
            if (boost::out_edges(v, _wfGraph).first == boost::out_edges(v, _wfGraph).second) {
                _outputNodes.push_back(v);
            } else {
                // some pouts may be unassigned
                std::vector<quint16> assignedPouts = getAssignedPouts(v);
                quint16 assignedOutputSize = assignedPouts.size();

                IOperationMetaData meta = getOperationMetadata(v);
                quint16 possibleOutputSize = meta->getOutputParameters().size();

                if (possibleOutputSize > assignedOutputSize) {
                    _outputNodes.push_back(v); // further parameter assignment possible
                }
            }
        }
    }
    return _outputNodes;
}

std::vector<quint16> Workflow::getAssignedPouts(const OVertex &v)
{
    std::vector<quint16> assignedPouts;
    boost::graph_traits<WorkflowGraph>::out_edge_iterator ei, ei_end;
    for (boost::tie(ei,ei_end) = getOutEdges(v); ei != ei_end; ++ei) {
        // implicitly assigned pins via edges
        assignedPouts.push_back(edgeProperties(*ei).outputIndexLastOperation);
    }
    for (SPOutputDataProperties output : _outputProperties[v]) {
        // explicitly assigned pins via edges
        assignedPouts.push_back(output->assignedParameterIndex);
    }
    return assignedPouts;
}

OVertex Workflow::getPreviousOperationNode(const OEdge &e)
{
    return boost::source(e, _wfGraph);
}

OVertex Workflow::getNextOperationNode(const OEdge &e)
{
    return boost::target(e, _wfGraph);
}

std::pair<OutEdgeIterator,OutEdgeIterator> Workflow::getOutEdges(const OVertex &v)
{
    return boost::out_edges(v, _wfGraph);
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
    // TODO allow multiple edges between v1 and v2?

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
    quint64 id = source().id();

    mastercatalog()->updateItems({source()});
    commandhandler()->addOperation(id, WorkflowOperationImplementation::create);
    return id;
}

NodePropertyMap Workflow::nodeIndex()
{
    return get(boost::vertex_index1, _wfGraph);
}

EdgePropertyMap Workflow::edgeIndex()
{
    return get(boost::edge_index, _wfGraph);
}

IOperationMetaData Workflow::getOperationMetadata(const OVertex &v)
{
    IOperationMetaData metadata;
    quint64 id = nodeProperties(v).id;
    metadata.prepare(mastercatalog()->id2Resource(id));
    if ( !metadata.isValid()) {
        qDebug() << "operation with id" << id << " is unknown!";
        return IOperationMetaData();
    }
    return metadata;
}

void Workflow::parseInputParameters()
{
    clearInputParameters();
    _inputNodes.clear();

    QStringList mandatoryInputs;
    QStringList optionalInputs;
    quint16 parameterIndex = 0;
    for (OVertex inputNode : getNodesWithExternalInput()) {
        std::vector<quint16> assignedPins = getAssignedPins(inputNode);
        QStringList inputTerms = getInputTerms(inputNode);

        // iterate over operation's pouts
        IOperationMetaData meta = getOperationMetadata(inputNode);
        for (int i = 0; i < meta->getInputParameters().size() ; i++) {
            SPOperationParameter input = meta->getInputParameters().at(i);
            auto iter = std::find(assignedPins.begin(), assignedPins.end(), i);
            if (iter == assignedPins.end()) {

                // TODO also do not add if shared input

                addParameter(input); // not yet assigned
                input->copyMetaToResourceOf(connector(), parameterIndex++);
                if (input->isOptional()) {
                    optionalInputs << inputTerms.at(i);
                } else {
                    mandatoryInputs << inputTerms.at(i);
                }

                //qDebug() << "added input parameter: ";
                //debugOperationParameter(parameter);
            }
        }
    }
    QString inparameters = createParametersCountString(mandatoryInputs, optionalInputs);
    connector()->setProperty("inparameters", inparameters);

    QString bracketOpen = mandatoryInputs.isEmpty() ? "[" : "[,";
    QString opts = !optionalInputs.isEmpty()
            ? bracketOpen + optionalInputs.join(",") + "]"
            : "";
    QString workflowSyntax = QString("%1(%2%3)").arg(name()).arg(mandatoryInputs.join(",")).arg(opts);
    connector()->setProperty("syntax", workflowSyntax);
}

void Workflow::parseOutputParameters()
{
    clearOutputParameters();
    _outputNodes.clear();

    QStringList mandatoryOutputs;
    QStringList optionalOutputs;
    quint16 parameterIndex = 1;
    for (OVertex outputNode : getNodesWithExternalOutputs()) {

        // TODO more readable syntax terms
        std::vector<quint16> assignedPouts = getAssignedPouts(outputNode);
        QStringList outputTerms = getOutputTerms(outputNode);

        // iterate over operation's pouts
        IOperationMetaData meta = getOperationMetadata(outputNode);
        for (int i = 0 ; i < meta->getOutputParameters().size() ; i++) {
            SPOperationParameter output = meta->getOutputParameters().at(i);
            auto iter = std::find(assignedPouts.begin(), assignedPouts.end(), i);
            if (iter == assignedPouts.end()) {
                addParameter(output); // not yet assigned
                output->copyMetaToResourceOf(connector(), parameterIndex++);
                if (output->isOptional()) {
                    optionalOutputs << outputTerms.at(i);
                } else {
                    mandatoryOutputs << outputTerms.at(i);
                }

                //qDebug() << "added output parameter: ";
                //debugOperationParameter(parameter);
            }
        }
    }
    QString outparameters = createParametersCountString(mandatoryOutputs, optionalOutputs);
    connector()->setProperty("outparameters", outparameters);
}

QStringList Workflow::getInputTerms(const OVertex &v)
{
    IOperationMetaData meta = getOperationMetadata(v);
    return createSyntaxTerms(v, meta->getInputParameters(), "_input_%1");
}

QStringList Workflow::getOutputTerms(const OVertex &v)
{
    IOperationMetaData meta = getOperationMetadata(v);
    return createSyntaxTerms(v, meta->getOutputParameters(), "_output_%1");
}

QStringList Workflow::createSyntaxTerms(const OVertex &v, const std::vector<SPOperationParameter> &parameters, const QString &inoutTemplate)
{
    QStringList list;
    NodeProperties properties = nodeProperties(v);
    QString opQualifier = QString::number(v) + "_" + QString::number(properties.id) + inoutTemplate;
    for (int i = 0 ; i < parameters.size() ; i++) {
        SPOperationParameter parameter = parameters.at(i);
        QString term = !parameter->term().isEmpty()
                ? parameter->term()
                : QString::number(i);
        list << QString(opQualifier).arg(term);
    }
    return list;
}

QString Workflow::createParametersCountString(const QStringList &mandatory, const QStringList &optionals) const
{
    quint16 minCount = mandatory.size();
    QString parametersCount = QString::number(minCount);
    for (int i = 1; i <= optionals.size() ; ++i) {
        if ( !parametersCount.startsWith("|")) {
            parametersCount += "|";
        }
        parametersCount.append(QString::number(minCount + i));
    }
    return parametersCount;
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

void Workflow::debugWorkflowMetadata() const
{
    qDebug() << "syntax: " << source()["syntax"].toString();
    qDebug() << "inparameters: " << source()["inparameters"].toString();
    qDebug() << "outparameters: " << source()["outparameters"].toString();
    qDebug() << "pins:";
    for (SPOperationParameter parameter : getInputParameters()) {
        debugOperationParameter(parameter);
    }
    qDebug() << "pouts:";
    for (SPOperationParameter parameter : getOutputParameters()) {
        debugOperationParameter(parameter);
    }
}

void Workflow::debugOperationParameter(const SPOperationParameter parameter) const
{
    qDebug() << "\tterm: " << parameter->term() << ", " << "(name: " << parameter->name() << ", optional: " << parameter->isOptional() << ")";
}
