#ifndef WORKFLOW_H
#define WORKFLOW_H

#include <QPoint>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "kernel_global.h"
#include "operationmetadata.h"

namespace Ilwis {

struct NodeProperties {
    QUrl url;
};

struct EdgeProperties {
    QString pin;
    QString pout;
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
    OVertex addOperation(const NodeProperties &properties);
    OEdge addOperationFlow(const OVertex &v1, const OVertex &v2, const EdgeProperties &properties);
    void removeOperation(OVertex vertex);
    void removeOperationFlow(OEdge edge);

    QList<OVertex> getRoots();
    QList<OVertex> getLeafs();
    NodeProperties operationProperties(const OVertex &v);

    void updateNodeProperties(OVertex v, const NodeProperties &properties);
    void updateEdgeProperties(OEdge e, const EdgeProperties &properties);

    // ------ operation metadata functions
    IlwisTypes ilwisType() const;
    quint64 createMetadata();

    // for debugging
    void debugPrintGraph();
    void debugPrintVertices();
    void debugPrintEdges();

private:
    WorkflowGraph _wfGraph;
    //QList<NodeRenderingProperties> _nodeRenderingProperties;
    //QList<EdgeRenderingProperties> _edgeRenderingProperties;

    NodePropertyMap nodeIndex();
    EdgePropertyMap edgeIndex();

    void parseInputParameters();
    void parseOutputParameters();

};

typedef IlwisData<Workflow> IWorkflow;

}

#endif // WORKFLOW_H
