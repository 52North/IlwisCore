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
    enum ConditionParts{cpTEST=0, cpTRUECASE=1, cpFALSECASE=2};
    struct Test{
        LogicalOperator _pre= loNONE;
        SPWorkFlowNode _operation;
        LogicalOperator _post = loNONE;

        bool isValid() const;
    };

    WorkFlowCondition();

    Ilwis::WorkFlowNode::NodeTypes type() const;

    std::vector<SPWorkFlowNode> subnodes(const QString& reason="") const;
    void addSubNode(const SPWorkFlowNode &node, const QString &reason);
    void removeSubNode(Ilwis::NodeId dNodeId);
    void addTest(SPWorkFlowNode node, LogicalOperator pre, LogicalOperator post);
    void setTestValue(int testIndex, int parameterIndex, const QString& value, const IWorkflow &wf);
    QString testValue(int testIndex, int parameterIndex, const IWorkflow &wf) const;
    Test test(int index) const;
    int testCount() const;
    void nodeId(quint64 id);
    bool isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck check) const;

private:
    std::vector<Test> _tests;
    std::vector<SPWorkFlowNode> _operations;
};
typedef std::shared_ptr<WorkFlowCondition> SPCondition;
}

#endif // WORKFLOWCONDITION_H
