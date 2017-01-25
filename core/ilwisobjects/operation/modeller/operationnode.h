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

    bool execute(ExecutionContext *ctx, SymbolTable &symTable, const OperationExpression &expression, const std::map<quint64, int> &idmap);
    IOperationMetaData operation() const;
    quint64 operationid() const;
    void operationid(quint64 iod) ;
    QString type() const;
    bool isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck vc) const ;

private:
    quint64 _operationid;
};
}

#endif // OPERATIONNODE_H
