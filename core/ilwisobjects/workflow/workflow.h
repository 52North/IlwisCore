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

struct DataProperties {
    DataProperties() {}
    //bool treatOptionalAsRequired; // TODO
    quint16 assignedParameterIndex;
    SPOperationParameter parameter;
};

typedef std::shared_ptr<DataProperties> SPDataProperties;

struct NodeProperties {
    quint64 id;
};

struct EdgeProperties {
    quint16 outputIndexLastStep;
    quint16 inputIndexNextStep;
};

typedef boost::property<boost::vertex_index1_t, NodeProperties> NodeProperty;
typedef boost::property<boost::edge_index_t, EdgeProperties> FlowProperty;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                              NodeProperty, FlowProperty> WorkflowGraph;

typedef boost::property_map<WorkflowGraph, boost::vertex_index1_t>::type NodePropertyMap;
typedef boost::property_map<WorkflowGraph, boost::edge_index_t>::type EdgePropertyMap;

typedef boost::graph_traits<WorkflowGraph>::vertex_descriptor OVertex;
typedef boost::graph_traits<WorkflowGraph>::edge_descriptor OEdge;

class KERNELSHARED_EXPORT Workflow: public OperationMetaData
{

public:
    Workflow();
    Workflow(const Resource &resource);
    ~Workflow();

    // ------ workflow API functions
    SPDataProperties addInputDataProperties(const OVertex v);
    SPDataProperties addOutputDataProperties(const OVertex &v);
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
    IlwisTypes ilwisType() const;
    quint64 createMetadata();

    // for debugging
    void debugPrintGraph();
    void debugPrintVertices();
    void debugPrintEdges();
    void debugWorkflowMetadata() const;
    void debugOperationParameter(const SPOperationParameter parameter) const;

private:
    WorkflowGraph _wfGraph;

    QMap<OVertex, QList<SPDataProperties>> _inputProperties;
    QMap<OVertex, QList<SPDataProperties>> _outputProperties;
    //QMap<OVertex, QList<OperationParameter>> _ioProperties;
    //QList<NodeRenderingProperties> _nodeRenderingProperties;
    //QList<EdgeRenderingProperties> _edgeRenderingProperties;

    NodePropertyMap nodeIndex();
    EdgePropertyMap edgeIndex();

    void parseInputParameters();
    void parseOutputParameters();
    IOperationMetaData getOperationMetadata(quint64 id) const;
    std::vector<quint16> getAssignedPins(const OVertex &v);
    std::vector<quint16> getAssignedPouts(const OVertex &v);

};

typedef IlwisData<Workflow> IWorkflow;

}

#endif // WORKFLOW_H
