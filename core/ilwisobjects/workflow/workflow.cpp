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

Workflow::Workflow (): OperationMetaData()
{
}

Workflow::Workflow(const Resource &resource): OperationMetaData(resource)
{
}

Workflow::~Workflow() {

}

bool Workflow::hasInputAssignments(const OVertex &v) const
{
    for (InputAssignment assignment : _inputAssignments.keys()) {
        if (assignment.first == v) {
            return true;
        }
    }
    return false;
}

bool Workflow::hasInputAssignment(const OVertex &v, int index) const
{
    return _inputAssignments.contains(std::make_pair(v, index));
}

SPAssignedInputData Workflow::getAssignedInputData(const InputAssignment &assignment) const
{
    return _inputAssignments.value(assignment);
}

SPAssignedInputData Workflow::assignInputData(const OVertex &v, quint16 index)
{
    SPAssignedInputData properties = SPAssignedInputData(new AssignedInputData());
    assignInputData({v, index}, properties);
    return properties;
}

void Workflow::assignInputData(const InputAssignment &assignment, const SPAssignedInputData &properties)
{
    _inputAssignments.insert(assignment, properties);
}

SPAssignedOutputData Workflow::addOutputDataProperties(const OVertex &v)
{
    if ( !_outputProperties.contains(v)) {
        QList<SPAssignedOutputData> set;
        _outputProperties.insert(v, set);
    }
    SPAssignedOutputData properties = SPAssignedOutputData(new AssignedOutputData());
    _outputProperties[v].push_back(properties);
    return properties;
}

QList<SPAssignedInputData> Workflow::getAssignedInputData(const OVertex &v) const
{
    if (hasInputAssignments(v)) {
        QList<SPAssignedInputData> list;
        for (InputAssignment assignment : _inputAssignments.keys()) {
            if (assignment.first == v) {
                list.push_back(_inputAssignments.value(assignment));
            }
        }
        return list;
    } else {
        QList<SPAssignedInputData> emptyList;
        return emptyList;
    }
}

QList<SPAssignedOutputData> Workflow::getOutputDataProperties(const OVertex &v) const
{
    if (_outputProperties.contains(v)) {
        return _outputProperties[v];
    } else {
        QList<SPAssignedOutputData> emptySet;
        return emptySet;
    }
}

void Workflow::removeInputAssignment(const OVertex &v, quint16 index)
{
    _inputAssignments.remove(std::make_pair(v, index));
}

void Workflow::removeAllInputAssignments(const OVertex &v)
{
    for (InputAssignment assignment : _inputAssignments.keys()) {
        if (assignment.first == v) {
            _inputAssignments.remove(assignment);
        }
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
    OVertex v = boost::add_vertex(properties, _wfGraph);

    IOperationMetaData meta = getOperationMetadata(v);
    std::vector<SPOperationParameter> inputs = meta->getInputParameters();
    for (int i = 0 ; i < inputs.size() ; i++) {
        if ( !inputs.at(i)->isOptional()) {
            // ensure required parameters has input assignment
            assignInputData(v, i);
        }
    }
    return v;
}

void Workflow::removeOperation(OVertex vertex)
{
    boost::remove_vertex(vertex, _wfGraph);
    removeAllInputAssignments(vertex);
}

NodeProperties Workflow::nodeProperties(const OVertex &v)
{
    return nodeIndex()[v];
}

EdgeProperties Workflow::edgeProperties(const OEdge &e)
{
    return edgeIndex()[e];
}

// Gets all nodes which have input parameters which are open.
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
                int assignedInputSize = getImplicitInputAssignments(v).size() + getConstantInputAssignments(v).size();

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

QList<InputAssignment> Workflow::getConstantInputAssignments(const OVertex &v) const
{
    QList<InputAssignment> assignedPins;
    for (InputAssignment assignment : _inputAssignments.keys()) {
        if (assignment.first == v) {
            if (_inputAssignments.value(assignment)->value.size() > 0) {
                assignedPins.push_back(assignment);
            }
        }
    }
    return assignedPins;
}

QList<InputAssignment> Workflow::getOpenInputAssignments(const OVertex &v) const
{
    QList<InputAssignment> assigned;
    for (InputAssignment assignment : _inputAssignments.keys()) {
        if (assignment.first == v) {
            assigned.push_back(assignment);
        }
    }
    return assigned;
}

QList<InputAssignment> Workflow::getImplicitInputAssignments(const OVertex &v)
{
    QList<InputAssignment> assignedPins;
    boost::graph_traits<WorkflowGraph>::in_edge_iterator ei, ei_end;
    for (boost::tie(ei,ei_end) = getInEdges(v); ei != ei_end; ++ei) {
        // internal pins
        InputAssignment assignment = std::make_pair(v, edgeProperties(*ei)._inputParameterIndex);
        assignedPins.push_back(assignment);
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
        assignedPouts.push_back(edgeProperties(*ei)._outputParameterIndex);
    }
    for (SPAssignedOutputData output : _outputProperties[v]) {
        // explicitly assigned pins via edges
        assignedPouts.push_back(output->assignedParameterIndex);
    }
    return assignedPouts;
}

OVertex Workflow::getSourceOperationNode(const OEdge &e)
{
    return boost::source(e, _wfGraph);
}

OVertex Workflow::getTargetOperationNode(const OEdge &e)
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

bool Workflow::hasValueDefined(const OVertex &operationVertex, int parameterIndex)
{
    boost::graph_traits<WorkflowGraph>::vertex_iterator vi, vi_end;
    // Loop through all vertices
    for (boost::tie(vi, vi_end) = boost::vertices(_wfGraph); vi != vi_end; ++vi) {
        // Does it exist?
        if ((*vi) == operationVertex) {
            // Does it have an edge asigned to it?
            for (const InputAssignment& assignment : getImplicitInputAssignments(operationVertex)) {
                if (assignment.first == operationVertex && assignment.second == parameterIndex) {
                   return true;
                }
            }
            return _inputAssignments.value({operationVertex, parameterIndex})->value.size() > 0;
        }
    }
    return false;
}

/**
 * Returns the inputs of an operation which have already been defined (if they for example have a flow drawn to them)
 * @param operationVertex the operations
 * @return A string seperated by |
 */
QString Workflow::definedValueIndexes(const OVertex &operationVertex){
    QString definedValues;

    for (const InputAssignment& assignment : getImplicitInputAssignments(operationVertex)) {
        if (assignment.first == operationVertex && hasValueDefined(operationVertex, assignment.second)) {
            if(!definedValues.isEmpty()){
                definedValues += "|";
            }
            definedValues += QString::number(assignment.second);
        }
    }

    return definedValues;
}

OEdge Workflow::addOperationFlow(const OVertex &from, const OVertex &to, const EdgeProperties &properties)
{
    // TODO allow multiple edges between v1 and v2?

    removeInputAssignment(to, properties._inputParameterIndex);
    return (boost::add_edge(from, to, properties, _wfGraph)).first;
}

void Workflow::removeOperationFlow(OEdge edge) {
    EdgeProperties edgeProps = edgeProperties(edge);
    assignInputData(boost::target(edge, _wfGraph), edgeProps._inputParameterIndex);
    boost::remove_edge(edge, _wfGraph);
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
    //mastercatalog()->addItems({source()});
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
    quint64 id = nodeProperties(v)._operationid;
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

    QList<SPAssignedInputData> sharedInputs;
    for (OVertex inputNode : getNodesWithExternalInput()) {
        QList<InputAssignment> implicitAssignments = getImplicitInputAssignments(inputNode);
        QList<InputAssignment> explicitAssignments = getConstantInputAssignments(inputNode);
        QStringList inputTerms = getInputTerms(inputNode);

        // iterate over operation's pins
        IOperationMetaData meta = getOperationMetadata(inputNode);
        for (int i = 0; i < meta->getInputParameters().size() ; i++) {
            //qDebug() << "nr of inputparameters";
            //qDebug() << meta->getInputParameters().size();
            InputAssignment candidate = std::make_pair(inputNode, i);

            if ( !implicitAssignments.contains(candidate) && !explicitAssignments.contains(candidate)) {
                SPOperationParameter input = meta->getInputParameters().at(i);

                QString term;
                if ( !hasInputAssignment(inputNode, i)) {
                    term = inputTerms.at(i);
                } else {
                    SPAssignedInputData inputData = getAssignedInputData({inputNode, i});
                    if (sharedInputs.contains(inputData)) {
                        continue;
                    } else {
                        inputData->inputName = inputData->inputName.isEmpty()
                                ? inputTerms.at(i) : inputData->inputName;
                        term = inputData->inputName;
                        sharedInputs.push_back(inputData);
                    }
                }
                addParameter(input); // not yet assigned
                input->copyMetaToResourceOf(connector(), parameterIndex++);

                //qDebug()<<"Copymetatoresourceof fisished";
                if (input->isOptional()) {
                    optionalInputs << term;
                } else {
                    mandatoryInputs << term;
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
    quint16 parameterIndex = 0;
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
    QString opQualifier = QString::number(v) + "_" + QString::number(properties._operationid) + inoutTemplate;
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
        qDebug() << p._operationid;// /*<< " ";//*/ << " (id=" << v << ") ";
    }
}

void Workflow::debugPrintEdges()
{
    qDebug() << "edges(g) = ";
    boost::graph_traits<WorkflowGraph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(_wfGraph); ei != ei_end; ++ei)
        qDebug() << "(" << nodeIndex()[boost::source(*ei, _wfGraph)]._operationid
                  << "->" << nodeIndex()[boost::target(*ei, _wfGraph)]._operationid
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
