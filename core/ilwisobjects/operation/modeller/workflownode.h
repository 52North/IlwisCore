#ifndef WORKFLOWINPUTPARAMETER_H
#define WORKFLOWINPUTPARAMETER_H
#include "kernel_global.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflowparameter.h"

namespace Ilwis {

class OperationExpression;

class KERNELSHARED_EXPORT WorkFlowNode : public Identity
{
public:

    WorkFlowNode(const QString &name, const QString &description="", quint64 id=i64UNDEF);

    virtual int inputCount() const;
    WorkFlowParameter& inputRef(int index);
    void addInput(const WorkFlowParameter& param, int index=iUNDEF);
    void removedInput(int index);
    void nodeId(quint64 id);
    void box(const BoundingBox& box);
    BoundingBox box() const;
    std::shared_ptr<WorkFlowNode> owner() const;
    void owner(std::shared_ptr<WorkFlowNode> own);

    virtual IOperationMetaData operation() const;
    virtual QString type() const = 0;
    virtual std::vector<std::shared_ptr<WorkFlowNode>> subnodes(const QString& reason="") const;
    virtual void addSubNode(const std::shared_ptr<WorkFlowNode>& node, const QString& reason);
    virtual bool execute(ExecutionContext *ctx, SymbolTable &symTable, const OperationExpression &expression, const std::map<quint64, int> &idmap);

protected:
    std::vector<WorkFlowParameter> _inputParameters1;
    BoundingBox _box;
    std::shared_ptr<WorkFlowNode> _owner;
};

typedef std::shared_ptr<WorkFlowNode> SPWorkFlowNode;

KERNELSHARED_EXPORT bool operator==(const Ilwis::SPWorkFlowNode& node1, const Ilwis::SPWorkFlowNode& node2 );
KERNELSHARED_EXPORT bool operator<(const Ilwis::SPWorkFlowNode& node1, const Ilwis::SPWorkFlowNode& node2 );
}

#endif // WORKFLOWINPUTPARAMETER_H
