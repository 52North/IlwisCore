#ifndef WORKFLOW_H
#define WORKFLOW_H

#include <QPoint>
#include <QMap>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "kernel_global.h"
#include "ilwistypes.h"
#include "operationmetadata.h"

namespace Ilwis {

struct InputDataProperties {
    InputDataProperties() {}
    quint16 assignedParameterIndex;
    QVariant value;
};

struct OutputDataProperties {
    OutputDataProperties() {}
    quint16 assignedParameterIndex;
    QString outputName;
};

typedef std::shared_ptr<InputDataProperties> SPInputDataProperties;
typedef std::shared_ptr<OutputDataProperties> SPOutputDataProperties;

struct NodeProperties {
    quint64 id;
};

struct EdgeProperties {
    QString outputName;
    bool temporary = true;
    quint16 outputIndexLastOperation;
    quint16 inputIndexNextOperation;
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

class KERNELSHARED_EXPORT Workflow: public OperationMetaData
{

    friend class WorkflowOperationImplementation;

public:
    Workflow();
    Workflow(const Resource &resource);
    ~Workflow();

    // ------ workflow API functions
    SPInputDataProperties addInputDataProperties(const OVertex &v);
    SPOutputDataProperties addOutputDataProperties(const OVertex &v);
    QList<SPInputDataProperties> getInputDataProperties(const OVertex &v) const;
    QList<SPOutputDataProperties> getOutputDataProperties(const OVertex &v) const;
    void removeInputDataProperties(const OVertex &v, quint16 index);
    void removeOutputDataProperties(const OVertex &v, quint16 index);

    OVertex addOperation(const NodeProperties &properties);
    OEdge addOperationFlow(const OVertex &v1, const OVertex &v2, const EdgeProperties &properties);
    void removeOperation(OVertex vertex);
    void removeOperationFlow(OEdge edge);

    QList<OVertex> getNodesWithExternalInput();
    QList<OVertex> getNodesWithExternalOutputs();

    NodeProperties nodeProperties(const OVertex &v);
    EdgeProperties edgeProperties(const OEdge &e);

    void updateNodeProperties(OVertex v, const NodeProperties &properties);
    void updateEdgeProperties(OEdge e, const EdgeProperties &properties);

    // ------ operation metadata functions
    IOperationMetaData getOperationMetadata(const OVertex &v);
    IlwisTypes ilwisType() const;
    quint64 createMetadata();

    // ------ for debugging
    void debugPrintGraph();
    void debugPrintVertices();
    void debugPrintEdges();
    void debugWorkflowMetadata() const;
    void debugOperationParameter(const SPOperationParameter parameter) const;

private:
    WorkflowGraph _wfGraph;
    QList<OVertex> _inputNodes;
    QList<OVertex> _outputNodes;

    QMap<OVertex, QList<SPInputDataProperties>> _inputProperties;
    QMap<OVertex, QList<SPOutputDataProperties>> _outputProperties;
    //QList<NodeRenderingProperties> _nodeRenderingProperties;
    //QList<EdgeRenderingProperties> _edgeRenderingProperties;

    NodePropertyMap nodeIndex();
    EdgePropertyMap edgeIndex();

    void parseInputParameters();
    void parseOutputParameters();
    QStringList createSyntaxTerms(const OVertex &v, const std::vector<SPOperationParameter> &parameters, const QString &inoutPart);
    QString createParametersCountString(const QStringList &mandatory, const QStringList &optionals) const;

    QStringList getInputTerms(const OVertex &v);
    QStringList getOutputTerms(const OVertex &v);
    std::vector<quint16> getAssignedPins(const OVertex &v);
    std::vector<quint16> getAssignedPouts(const OVertex &v);

    OVertex getPreviousOperationNode(const OEdge &e);
    OVertex getNextOperationNode(const OEdge &e);
    std::pair<InEdgeIterator, InEdgeIterator> getInEdges(const OVertex &v);
    std::pair<OutEdgeIterator, OutEdgeIterator> getOutEdges(const OVertex &v);
};

typedef IlwisData<Workflow> IWorkflow;

}

#endif // WORKFLOW_H
