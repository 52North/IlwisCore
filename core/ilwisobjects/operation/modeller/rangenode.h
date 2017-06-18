#ifndef LOOPNODE_H
#define LOOPNODE_H

#include "ilwis.h"

namespace Ilwis {

class KERNELSHARED_EXPORT RangeNode : public WorkFlowNode
{
public:
    RangeNode();

    Ilwis::WorkFlowNode::NodeTypes type() const;
    bool isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck check) const;
    void nodeId(quint64 id);
private:
    std::vector<SPWorkFlowNode> _operations;


};

typedef std::shared_ptr<RangeNode> SPLRangeNode;
}

#endif // LOOPNODE_H
