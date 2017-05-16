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
    QString parameterValue(int parmIndex) const;
    int parameterCount() const;

private:
    std::vector<QString> _parameterValues;
    SPWorkFlowNode _node;

    bool executeOperation(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, Ilwis::WorkflowIdMapping &mapping);
    bool executeCondition(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping &mapping);
    bool executeJunction(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping &mapping);
    bool executeContent(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);
    bool executeLoop(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);
    bool next();

    void unloadInputs(const std::vector<QString>& objects);
};
}

#endif // EXECUTIONNODE_H
