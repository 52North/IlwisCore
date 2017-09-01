#ifndef LOOPNODE_H
#define LOOPNODE_H

#include "ilwis.h"

namespace Ilwis {

class WorkFlowNode;
class RangeTestNode;
typedef std::shared_ptr<RangeTestNode> SPRangeTestNode;

class KERNELSHARED_EXPORT RangeNode : public WorkFlowNode
{
public:
    enum RangeParts{rpOUTPUT=0, rpINITIALINPUT=1, rpINPUT=2 ,rpFINALOUTPUT=3};
    RangeNode();
    Ilwis::WorkFlowNode::NodeTypes type() const;
    bool isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck check) const;

    void addSubNode(const std::shared_ptr<WorkFlowNode> &node, const QString &reason);
    void removeSubNode(Ilwis::NodeId nodeid);
    std::vector<std::shared_ptr<WorkFlowNode>> subnodes(const QString& reason="") const;
    void nodeId(quint64 id);
    int inputCount() const;
    void setRangeDefinition(const QString& val, const Ilwis::Workflow *workflow);
    QString rangeDefinition() const;
    bool next();
    QVariant currentValue() const;
private:
    std::vector<SPWorkFlowNode> _operations;
    std::vector<SPWorkFlowNode> _junctions;
    enum ContentCases{ccVECTOR, ccLIMITS, ccOBJECT, ccUNKNOWN};
    double _rangeStart = rUNDEF;
    double _rangeEnd = rUNDEF;
    double _precision = 1;
    std::vector<QString> _rangeValues;
    ContentCases _case = ccUNKNOWN;
    int _currentIndex = iUNDEF;
    double _currentValue = rUNDEF;
    QString _rangeDef;
};

typedef std::shared_ptr<RangeNode> SPLRangeNode;
}

#endif // LOOPNODE_H
