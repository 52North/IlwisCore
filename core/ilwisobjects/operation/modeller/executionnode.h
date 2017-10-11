#ifndef EXECUTIONNODE_H
#define EXECUTIONNODE_H

namespace Ilwis {
class WorkflowImplementation;
class WorkflowIdMapping;

struct IdTypePair;

typedef std::pair<IdTypePair, IdTypePair> FlowContext;

class ExecutionNode
{
public:
    ExecutionNode();
    ExecutionNode(const SPWorkFlowNode& node, Ilwis::WorkflowIdMapping &mapping);

    bool execute(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping &mapping);
    void clearCalculatedValues();

    QVariant parameterValue(int parmIndex) const;
    void parameterValue(int parmIndex, const QVariant& v) ;
    int parameterCount() const;
    Ilwis::SymbolTable &symbolTableRef();
    static FlowContext contextSwitch(const SPWorkFlowNode &sourceNode, const SPWorkFlowNode &targetNode);

private:
    std::vector<QVariant> _parameterValues;
    SPWorkFlowNode _node;
    SymbolTable _symTable;

    bool executeOperation(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, Ilwis::WorkflowIdMapping &mapping);
    bool executeTest(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping &mapping);
    bool executeJunction(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping &mapping);
    bool executeRange(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);

    void unloadInputs(const std::vector<QString>& objects);
    bool executeRangeJunction(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);
    bool executeRangeTestNode(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);

    void clearCalculatedValues(std::vector<SPWorkFlowNode>& operations, WorkflowImplementation *workflowImpl);
    void clearScopedCalcutedValues();

    bool executeLink(ExecutionContext *ctx, SymbolTable &symTable, Ilwis::WorkFlowParameter &paramete, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);
    void setInputJunction(ExecutionContext *ctx, SymbolTable &symTable, const SPWorkFlowNode &operation, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);
};
}

Q_DECLARE_METATYPE(Ilwis::FlowContext)

#endif // EXECUTIONNODE_H
