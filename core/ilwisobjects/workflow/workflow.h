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

    /**
     * @brief init initializes the workflow by generating OperationResource (incl. registering to the mastercatalog) and
     * adding the workflow as operation to the commandhandler.
     * @return the id of the registered operation.
     */
    quint64 init();

    IlwisTypes ilwisType() const;
};

typedef IlwisData<Workflow> IWorkflow;

}

#endif // WORKFLOW_H
