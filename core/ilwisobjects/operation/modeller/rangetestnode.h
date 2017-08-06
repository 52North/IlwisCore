#ifndef RANGETESTNODE_H
#define RANGETESTNODE_H


#include "ilwis.h"

namespace Ilwis {

class WorkFlowNode;

class KERNELSHARED_EXPORT RangeTestNode : public WorkFlowNode
{
public:
    RangeTestNode();
    Ilwis::WorkFlowNode::NodeTypes type() const;
    bool isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck check) const;
    void setRangeDefinition(const QVariant& val);
    bool next();
    QVariant currentValue() const;
private:
    enum ContentCases{ccVECTOR, ccLIMITS, ccOBJECT, ccUNKNOWN};
    double _rangeStart = rUNDEF;
    double _rangeEnd = rUNDEF;
    double _precision = 1;
    std::vector<QString> _rangeValues;
    ContentCases _case = ccUNKNOWN;
    int _currentIndex = iUNDEF;
    double _currentValue = rUNDEF;

};

typedef std::shared_ptr<RangeTestNode> SPRangeTestNode;
}

#endif // RANGETESTNODE_H
