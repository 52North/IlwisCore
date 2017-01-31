#ifndef LOOPNODE_H
#define LOOPNODE_H

#include "ilwis.h"

namespace Ilwis {

class KERNELSHARED_EXPORT LoopNode : public WorkFlowCondition
{
public:
    LoopNode();

    Ilwis::WorkFlowNode::NodeTypes type() const;
    bool execute(ExecutionContext *ctx, SymbolTable &symTable, const OperationExpression &expression, const std::map<quint64, int> &idmap);
    bool isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck check) const;
};
}

#endif // LOOPNODE_H
