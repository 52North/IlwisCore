#ifndef EXECUTIONNODE_H
#define EXECUTIONNODE_H

namespace Ilwis {
class WorkflowImplementation;

class ExecutionNode
{
public:
    ExecutionNode();
    ExecutionNode(const SPWorkFlowNode& node);

    bool execute(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, const OperationExpression& expression, const std::map<quint64, int>& idmap);
    void clearCalculatedValues();

private:
    std::vector<QString> _parameterValues;
    SPWorkFlowNode _node;

    bool executeOperation(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, const OperationExpression& expression, const std::map<quint64, int>& idmap);
    bool executeCondition(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, const OperationExpression& expression, const std::map<quint64, int>& idmap);
    bool executeJunction(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, const OperationExpression& expression, const std::map<quint64, int>& idmap);
    bool executeContent(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, const OperationExpression &expression, const std::map<quint64, int> &idmap);
    bool executeLoop(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, const OperationExpression &expression, const std::map<quint64, int> &idmap);
    bool next();
    QString parameterValue(int parmIndex) const;
};
}

#endif // EXECUTIONNODE_H
