#ifndef WORKFLOW_H
#define WORKFLOW_H

#include <map>
#include "kernel_global.h"
#include "operationExpression.h"

namespace Ilwis {

class WorkFlowNode;
class WorkFlowParameter;
class ExecutionNode;

typedef std::shared_ptr<WorkFlowNode> SPWorkFlowNode;

typedef std::map<SPWorkFlowNode, std::vector<SPWorkFlowNode>>::iterator WorkFlowMIter;
typedef std::map<SPWorkFlowNode, std::vector<SPWorkFlowNode>>::const_iterator CWorkFlowMIter;
typedef std::vector<SPWorkFlowNode>::iterator WorkFlowVIter;

class WorkflowIdMapping{
public :
    WorkflowIdMapping(const OperationExpression& expr, const std::map<quint64, int>& mapping);
    QVariant getValue(WorkFlowParameter& parm,const ExecutionNode& exnode) const;
    void advanceOffset(int n);
    int offset() const;
private:
    OperationExpression _expression;
    std::map<quint64, int> _mapping;
    int _offset = 0;  //subworkflows move an offset over the parameters; this offset determines were we are
};

class KERNELSHARED_EXPORT Workflow : public OperationMetaData
{
public:

    struct ExecutionOrder {
        std::vector<SPWorkFlowNode> _independentOrder;
        std::map<NodeId, std::vector<SPWorkFlowNode>> _dependentOrder;
    };

    Workflow();
    Workflow(const Resource &resource);

    NodeId addNode(SPWorkFlowNode node, NodeId parent = i64UNDEF);
    SPWorkFlowNode nodeById(NodeId id);
    const SPWorkFlowNode nodeById(NodeId id) const;
    void removeNode(NodeId id);
    std::vector<SPWorkFlowNode> outputNodes();
    void setFixedParameter(const QString &data, NodeId nodeId, qint32 parmIndex);
    void addFlow(NodeId fromNode, NodeId toNode, qint32 inParmIndex, qint32 outParmIndex, int attachRctIndxFrom, int attachRctIndxTo);
    void addConditionFlow(NodeId fromNode, NodeId toNode, qint32 testIndex, qint32 inParmIndex, qint32 outParmIndex, int attachRctIndxFrom, int attachRctIndxTo);
    void removeFlow(NodeId toNode, qint32 parameterIndex);
    void addJunctionFlow(int junctionIdTo, const QString &operationIdFrom, int paramIndex, int recFrom, int rectTo, bool truecase);
    qint32 operationInputParameterCount(NodeId nodeId);
    qint32 operationOutputParameterCount(NodeId nodeId);
    bool isParameterValueDefined(NodeId node, qint32 parameterIndex) const;
    quint64 createMetadata();
    quint64 createMetadata(int offset);

    IlwisTypes ilwisType() const;
    std::map<quint64, int> parmid2order() const;
    double scale() const;
    void scale(double s);
    std::pair<int, int> translation() const;
    void translation(double x, double y, bool relative);
    quint32 generateId();
    void updateIdCounter();
    const std::vector<SPWorkFlowNode>& graph() const;
    const std::vector<SPWorkFlowNode> nodes(int filter=WorkFlowNode::ntALL) const;
    bool isValid() const;
    static std::vector<SPWorkFlowNode> outputNodes(const std::vector<Ilwis::SPWorkFlowNode> graph, Workflow *flow);
     std::vector<SPWorkFlowNode> leafNodes(NodeId containerId);
private:
    std::vector<SPWorkFlowNode> _graph;
    std::map<quint64,int> _parmid2order; // some workflow parameters are bound to parameters in  the workflow expression; this describes that relation. It is filled in the createMetadata
    quint32 _idCounter = 0;
    OperationResource _operation;
    double _scale = 1.0;
    std::pair<int, int> _translation = std::pair<int, int>(0,0);


    std::vector<WorkFlowParameter> freeInputParameters() const;
    std::vector<Ilwis::SPOperationParameter> freeOutputParameters() const;



    void reworkInputNames(SPWorkFlowNode &node);
};
typedef IlwisData<Workflow> IWorkflow;
}

#endif // WORKFLOW_H
