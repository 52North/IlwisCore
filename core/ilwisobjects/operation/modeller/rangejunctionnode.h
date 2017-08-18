#ifndef RANGEJUNCTIONNODE_H
#define RANGEJUNCTIONNODE_H

#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT RangeJunctionNode : public WorkFlowNode
{
public:
    RangeJunctionNode();
    RangeJunctionNode(quint64 nodeid);
    Ilwis::WorkFlowNode::NodeTypes type() const;
    int inputCount() const;
    void link2initialInput(SPWorkFlowNode trueNode, int parmIndex);
    void link2Input(SPWorkFlowNode falseNode, int parmIndex);
    void link2Output(Ilwis::SPWorkFlowNode node, int parmIndex);
    void link2FinalOutput(Ilwis::SPWorkFlowNode node, int parmIndex);
    void nodeId(quint64 id);
    bool isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck) const;

private:
};

typedef std::shared_ptr<RangeJunctionNode> SPRangeJunctionNode;
}

#endif // RANGEJUNCTIONNODE_H
