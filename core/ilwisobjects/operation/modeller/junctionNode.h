#ifndef JUNCTION_H
#define JUNCTION_H

#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT JunctionNode : public WorkFlowNode
{
public:
    JunctionNode();
    JunctionNode(quint64 nodeid);
    Ilwis::WorkFlowNode::NodeTypes type() const;
    int inputCount() const;
    void link2trueCase(SPWorkFlowNode trueNode, int parmIndex);
    void link2falseCase(SPWorkFlowNode falseNode, int parmIndex);
    void link2condition(Ilwis::SPWorkFlowNode conditionnode);
    void nodeId(quint64 id);
    bool isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck) const;

private:
};

typedef std::shared_ptr<JunctionNode> SPJunctionNode;
}

#endif // JUNCTION_H
