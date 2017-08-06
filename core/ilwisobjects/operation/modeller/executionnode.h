#ifndef EXECUTIONNODE_H
#define EXECUTIONNODE_H

namespace Ilwis {
class WorkflowImplementation;
class WorkflowIdMapping;

class ExecutionNode
{
public:
    ExecutionNode();
    ExecutionNode(const SPWorkFlowNode& node, Ilwis::WorkflowIdMapping &mapping);

    bool execute(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping &mapping);
    void clearCalculatedValues();
    QVariant parameterValue(int parmIndex) const;
    int parameterCount() const;

private:
    std::vector<QVariant> _parameterValues;
    SPWorkFlowNode _node;

    bool executeOperation(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, Ilwis::WorkflowIdMapping &mapping);
    bool executeTest(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping &mapping);
    bool executeJunction(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping &mapping);
    bool executeRange(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);

    void unloadInputs(const std::vector<QString>& objects);
    bool executeRangeJunction(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);
    bool executeRangeTestNode(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);
};
}

#endif // EXECUTIONNODE_H
