#ifndef WORKFLOWINPUTPARAMETER_H
#define WORKFLOWINPUTPARAMETER_H
#include "kernel_global.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflowparameter.h"

namespace Ilwis {

typedef quint64 NodeId;

class OperationExpression;
class Workflow;
typedef IlwisData<Workflow> IWorkflow;
typedef std::shared_ptr<WorkFlowNode> SPWorkFlowNode;

class KERNELSHARED_EXPORT WorkFlowNode : public Identity
{
public:
    enum NodeTypes{ ntUNDEFINED=0, ntOPERATION=1, ntCONDITION=2, ntJUNCTION=4, ntRANGE=8, ntRANGEJUNCTION=16,ntRANGETESTNODE=32, ntNONE=64,ntALL=255};
    enum ValidityCheck{vcALLDEFINED,vcPARTIAL, vcTESTS, vcOPERATIONS, vcJUNCTIONS, vcAGGREGATE};
    WorkFlowNode(const QString &name, const QString &description="", quint64 id=i64UNDEF);

    virtual int inputCount() const;
    WorkFlowParameter& inputRef(int index);
    WorkFlowParameter input(int index) const;
    void addInput(const WorkFlowParameter& param, int index=iUNDEF);
    virtual void setFlow(const SPWorkFlowNode& fromNode, qint32 inParmIndex, qint32 outParmIndex, int attachRctIndxFrom, int attachRctIndxTo);
    void removedInput(int index);
    virtual void nodeId(quint64 id);
    void box(const BoundingBox& box);
    BoundingBox box() const;
    bool collapsed() const;
    void collapsed(bool yesno);
    std::shared_ptr<WorkFlowNode> owner() const;
    void owner(std::shared_ptr<WorkFlowNode> own);
    virtual QString label() const;
    void label(const QString& lbl);
    NodeId conditionIdOfTest() const;
    void conditionIdOfTest(NodeId id);

    virtual IOperationMetaData operation() const;
    virtual bool isWorkflow() const;
    virtual Ilwis::WorkFlowNode::NodeTypes type() const = 0;
    virtual std::vector<std::shared_ptr<WorkFlowNode>> subnodes(const QString& reason="") const;
    virtual void addSubNode(const std::shared_ptr<WorkFlowNode>& node, const QString& reason);
    virtual void removeSubNode(NodeId id);
    virtual bool isValid(const Workflow* workflow, WorkFlowNode::ValidityCheck) const = 0;

protected:
    std::vector<WorkFlowParameter> _inputParameters1;
    BoundingBox _box;
    QString _label;
    bool _collapsed = false;
    NodeId _conditionIdOfTest = i64UNDEF;
    std::shared_ptr<WorkFlowNode> _owner;
    bool checkLinkDefintion(const QString& linkDef, NodeId &nid, int &outParameterIndex) const;
};

struct IdTypePair {
     IdTypePair(){}
    IdTypePair(NodeId nid, WorkFlowNode::NodeTypes tp) : _nodeid(nid), _nodeType(tp){}
    NodeId _nodeid = i64UNDEF;
    WorkFlowNode::NodeTypes _nodeType = WorkFlowNode::ntUNDEFINED;
};



KERNELSHARED_EXPORT bool operator==(const Ilwis::IdTypePair& node1, const Ilwis::IdTypePair& node2 );

KERNELSHARED_EXPORT bool operator==(const Ilwis::SPWorkFlowNode& node1, const Ilwis::SPWorkFlowNode& node2 );
KERNELSHARED_EXPORT bool operator<(const Ilwis::SPWorkFlowNode& node1, const Ilwis::SPWorkFlowNode& node2 );
}

#endif // WORKFLOWINPUTPARAMETER_H
