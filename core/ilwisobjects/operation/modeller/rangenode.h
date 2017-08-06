#ifndef LOOPNODE_H
#define LOOPNODE_H

#include "ilwis.h"

namespace Ilwis {

class WorkFlowNode;
class RangeTestNode;
typedef std::shared_ptr<RangeTestNode> SPRangeTestNode;

class KERNELSHARED_EXPORT RangeNode : public WorkFlowNode
{
public:
    RangeNode();
    Ilwis::WorkFlowNode::NodeTypes type() const;
    bool isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck check) const;

    void addSubNode(const std::shared_ptr<WorkFlowNode> &node, const QString &reason);
    std::vector<std::shared_ptr<WorkFlowNode>> subnodes(const QString& reason="") const;
    void nodeId(quint64 id);
    bool next();
    void test(RangeTestNode *);
    const Ilwis::SPRangeTestNode test() const;
private:
    std::vector<SPWorkFlowNode> _operations;
    SPRangeTestNode _test;


};

typedef std::shared_ptr<RangeNode> SPLRangeNode;
}

#endif // LOOPNODE_H
