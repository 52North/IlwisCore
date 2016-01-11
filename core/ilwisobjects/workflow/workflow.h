#ifndef WORKFLOW_H
#define WORKFLOW_H

#include <QPoint>
#include <QMap>
#include <QUrl>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "kernel_global.h"
#include "ilwistypes.h"
#include "operationmetadata.h"

namespace Ilwis {

struct AssignedInputData {
    AssignedInputData() {}
    QString inputName;
    QString value;
};

struct AssignedOutputData {
    AssignedOutputData() {}
    quint16 assignedParameterIndex;
    QString outputName;
};

typedef std::shared_ptr<AssignedInputData> SPAssignedInputData;
typedef std::shared_ptr<AssignedOutputData> SPAssignedOutputData;

struct NodeProperties {
    NodeProperties(){}
    NodeProperties(const Resource& res) : _operationid(res.id()){
        _syntax = res["syntax"].toString();
        _resourceProvider = res["namespace"].toString();
    }
    NodeProperties(const QString& syntax, QString provider, quint16 x, quint16 y) {
        _syntax = syntax;
        _resourceProvider = provider;
        _x = x;
        _y = y;
        std::vector<Resource> items = mastercatalog()->select("catalogitemproperties.propertyname='syntax' and catalogitemproperties.propertyvalue='" + syntax + "'");
        if ( items.size() == 1){
            _operationid = items[0].id();
        }
        qDebug() << _operationid;
    }
    quint64 _operationid = i64UNDEF;
    QString _syntax;
    QString _resourceProvider;
    quint16 _x;
    quint16 _y;
};

struct EdgeProperties {
    EdgeProperties(int outParm, int inParm, int inRect, int outRect) :
        _outputParameterIndex(outParm),
        _inputParameterIndex(inParm),
        _outputRectangleIndex(outRect),
        _inputRectangleIndex(inRect){}
    QString outputName;
    bool temporary = true;
    int _outputParameterIndex;
    int _inputParameterIndex;
    int _outputRectangleIndex;
    int _inputRectangleIndex;
};

typedef boost::property<boost::vertex_index1_t, NodeProperties> NodeProperty;
typedef boost::property<boost::edge_index_t, EdgeProperties> FlowProperty;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                              NodeProperty, FlowProperty> WorkflowGraph;

typedef boost::property_map<WorkflowGraph, boost::vertex_index1_t>::type NodePropertyMap;
typedef boost::property_map<WorkflowGraph, boost::edge_index_t>::type EdgePropertyMap;

typedef boost::graph_traits<WorkflowGraph>::vertex_descriptor OVertex;
typedef boost::graph_traits<WorkflowGraph>::edge_descriptor OEdge;

typedef boost::graph_traits<WorkflowGraph>::in_edge_iterator InEdgeIterator;
typedef boost::graph_traits<WorkflowGraph>::out_edge_iterator OutEdgeIterator;

typedef boost::graph_traits<WorkflowGraph>::vertex_iterator WorkflowVertexIterator;

typedef std::pair<OVertex, int> InputAssignment;

class KERNELSHARED_EXPORT Workflow: public OperationMetaData
{

    friend class WorkflowOperationImplementation;

public:
    Workflow();
    Workflow(const Resource &resource);
    ~Workflow();

    // ------ workflow API functions
    bool hasInputAssignments(const OVertex &v) const;
    bool hasInputAssignment(const OVertex &v, int index) const;
    SPAssignedInputData getAssignedInputData(const InputAssignment &assignment) const;
    SPAssignedInputData assignInputData(const OVertex &v, quint16 index);
    void assignInputData(const InputAssignment &assignment, const SPAssignedInputData &properties);

    SPAssignedOutputData addOutputDataProperties(const OVertex &v);
    QList<SPAssignedInputData> getAssignedInputData(const OVertex &v) const;
    QList<SPAssignedOutputData> getOutputDataProperties(const OVertex &v) const;
    void removeInputAssignment(const OVertex &v, quint16 index);
    void removeAllInputAssignments(const OVertex &v);
    void removeOutputDataProperties(const OVertex &v, quint16 index);

    OVertex addOperation(const NodeProperties &properties);
    OEdge addOperationFlow(const OVertex &from, const OVertex &to, const EdgeProperties &properties);
    void removeOperation(OVertex vertex);
    void removeOperationFlow(OEdge edge);

    QList<OVertex> getNodesWithExternalInput();
    QList<OVertex> getNodesWithExternalOutputs();

    NodeProperties nodeProperties(const OVertex &v);
    EdgeProperties edgeProperties(const OEdge &e);

    void updateNodeProperties(OVertex v, const NodeProperties &properties);
    void updateEdgeProperties(OEdge e, const EdgeProperties &properties);

    OVertex getSourceOperationNode(const OEdge &e);
    OVertex getTargetOperationNode(const OEdge &e);

    std::pair<InEdgeIterator, InEdgeIterator> getInEdges(const OVertex &v);
    std::pair<OutEdgeIterator, OutEdgeIterator> getOutEdges(const OVertex &v);

    //------- Queries
    bool hasValueDefined(const OVertex& operationVertex, int parameterIndex);

    QString definedValueIndexes(const OVertex &operationVertex);

    // ------ operation metadata functions
    IOperationMetaData getOperationMetadata(const OVertex &v);
    IlwisTypes ilwisType() const;
    quint64 createMetadata();

    // ------ Methods for saving
    QMap<InputAssignment, SPAssignedInputData> getAllInputAssignments() { return _inputAssignments; }
    std::pair<WorkflowVertexIterator, WorkflowVertexIterator> getNodeIterators() { return boost::vertices(_wfGraph); }

    // ------ for debugging
    void debugPrintGraph();
    void debugPrintVertices();
    void debugPrintEdges();
    void debugWorkflowMetadata() const;
    void debugOperationParameter(const SPOperationParameter parameter) const;

    virtual bool isInternalObject() const;

private:
    WorkflowGraph _wfGraph;
    QList<OVertex> _inputNodes;
    QList<OVertex> _outputNodes;

    QMap<InputAssignment, SPAssignedInputData> _inputAssignments;
    QMap<OVertex, QList<SPAssignedOutputData>> _outputProperties;

    NodePropertyMap nodeIndex();
    EdgePropertyMap edgeIndex();

    void parseInputParameters();
    void parseOutputParameters();
    QStringList createSyntaxTerms(const OVertex &v, const std::vector<SPOperationParameter> &parameters, const QString &inoutPart);
    QString createParametersCountString(const QStringList &mandatory, const QStringList &optionals) const;

    QStringList getInputTerms(const OVertex &v);
    QStringList getOutputTerms(const OVertex &v);
    QList<InputAssignment> getConstantInputAssignments(const OVertex &v) const;
    QList<InputAssignment> getOpenInputAssignments(const OVertex &v) const;
    QList<InputAssignment> getImplicitInputAssignments(const OVertex &v);
    std::vector<quint16> getAssignedPouts(const OVertex &v);
};

typedef IlwisData<Workflow> IWorkflow;

}

#endif // WORKFLOW_H
