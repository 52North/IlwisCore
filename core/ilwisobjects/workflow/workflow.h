#ifndef WORKFLOW_H
#define WORKFLOW_H

#include "boost/graph/graph_traits.hpp"
#include "boost/graph/adjacency_list.hpp"

#include "kernel_global.h"

using namespace boost;
typedef adjacency_list<vecS, vecS, directedS> ExecutionBranch;

namespace Ilwis {

class KERNELSHARED_EXPORT Workflow: public OperationMetaData
{
public:
    Workflow();
    Workflow(const Resource &resource);
    ~Workflow();

    static quint64 createMetadata();
};

typedef IlwisData<Workflow> IWorkflow;

}

#endif // WORKFLOW_H
