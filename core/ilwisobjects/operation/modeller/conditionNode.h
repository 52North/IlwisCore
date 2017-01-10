#ifndef WORKFLOWCONDITION_H
#define WORKFLOWCONDITION_H

#include "ilwis.h"

namespace Ilwis {

class Workflow;
typedef IlwisData<Workflow> IWorkflow;

class WorkFlowNode;
typedef std::shared_ptr<WorkFlowNode> SPWorkFlowNode;

class KERNELSHARED_EXPORT WorkFlowCondition : public WorkFlowNode
{
public:
    struct Test{
        LogicalOperator _pre= loNONE;
        SPWorkFlowNode _operation;
        LogicalOperator _post = loNONE;
    };

    WorkFlowCondition();

    QString type() const;

    std::vector<SPWorkFlowNode> subnodes(const QString& reason="") const;
    void addSubNode(const SPWorkFlowNode &node, const QString &reason);
    bool execute(ExecutionContext *ctx, SymbolTable &symTable, const OperationExpression &expression, const std::map<quint64, int> &idmap);
    void addTest(SPWorkFlowNode node, LogicalOperator pre, LogicalOperator post);
    void setTestValue(int testIndex, int parameterIndex, const QString& value, const IWorkflow &wf);
    QString testValue(int testIndex, int parameterIndex, const IWorkflow &wf) const;
    Test test(int index) const;
    int testCount() const;

private:
    std::vector<Test> _tests;
    std::vector<SPWorkFlowNode> _operations;
};
}

#endif // WORKFLOWCONDITION_H
