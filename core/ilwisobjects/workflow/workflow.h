#ifndef WORKFLOW_H
#define WORKFLOW_H

#include <QObject>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "kernel_global.h"
#include "operationmetadata.h"

namespace Ilwis {

struct OperationProperties {
    quint64 ilwisId;
};

struct FlowProperties {
    QString foo = "bar";
};

typedef boost::property<boost::vertex_index1_t, OperationProperties> NodeProperty;
typedef boost::property<boost::edge_index_t, FlowProperties> FlowProperty;
typedef boost::adjacency_list<boost::vecS, boost::vecS,
                              boost::bidirectionalS,
                              NodeProperty, FlowProperty> WorkflowGraph;

typedef boost::property_map<WorkflowGraph, boost::vertex_index1_t>::type NodePropertyMap;
typedef boost::property_map<WorkflowGraph, boost::edge_index_t>::type EdgePropertyMap;

typedef boost::graph_traits<WorkflowGraph>::vertex_descriptor OVertex;
typedef boost::graph_traits<WorkflowGraph>::edge_descriptor OEdge;


class KERNELSHARED_EXPORT Workflow: public OperationMetaData
{

private:
    Workflow();

public:
    Workflow(const Resource &resource);
    ~Workflow();

    // ------ workflow API functions
    OVertex addOperation(OperationProperties opProperties);
    void removeOperation(OVertex vertex);
    OEdge addOperationFlow(OVertex v1, OVertex v2, FlowProperties flowProperties);
    void removeOperationFlow(OEdge edge);

    // ------ operation metadata functions
    IlwisTypes ilwisType() const;
    quint64 createMetadata();

private:
    WorkflowGraph _wfGraph;
};

typedef IlwisData<Workflow> IWorkflow;

}

#endif // WORKFLOW_H
