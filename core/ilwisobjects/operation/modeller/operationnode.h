#ifndef OPERATIONNODE_H
#define OPERATIONNODE_H

#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT OperationNode : public WorkFlowNode
{
public:
    OperationNode();
    OperationNode(quint64 operationid);
    OperationNode(const QString &name, const QString &description="", quint64 id=i64UNDEF);

    IOperationMetaData operation() const;
    void operation(const QString& provider, const QString& syntax, bool isWorkflow);
    Ilwis::WorkFlowNode::NodeTypes type() const;
    bool isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck vc) const ;
    QString label() const;
    bool isWorkflow() const;

private:
    IOperationMetaData _operation;
    QString _syntax;
    QString _provider;
    bool _isWorkflow = false;
};
typedef std::shared_ptr<OperationNode> SPOperationNode;
}



#endif // OPERATIONNODE_H
