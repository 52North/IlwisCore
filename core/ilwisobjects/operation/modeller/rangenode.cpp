#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "operationExpression.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "operation.h"
#include "workflownode.h"
#include "workflow.h"
#include "mastercatalog.h"
#include "operationhelper.h"
#include "executionnode.h"
#include "rangetestnode.h"
#include "rangenode.h"

using namespace Ilwis;


RangeNode::RangeNode() : WorkFlowNode("")
{
}

Ilwis::WorkFlowNode::NodeTypes Ilwis::RangeNode::type() const
{
    return WorkFlowNode::ntRANGE;
}

bool RangeNode::isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck check) const
{
    return true;
}

void RangeNode::addSubNode(const std::shared_ptr<WorkFlowNode> &node, const QString &reason)
{
    if (reason == "operations"){
        if ( node->id() == i64UNDEF){
            kernel()->issues()->log(TR("Attempt to add invalid node to workflow; it has no id "));
            return;
        }
        _operations.push_back(node);
    }
}

std::vector<std::shared_ptr<WorkFlowNode> > RangeNode::subnodes(const QString &reason) const
{
    if ( reason == "operations")
        return _operations;
    std::vector<SPWorkFlowNode > all(_operation.begin(), _operations.end());
    all.insert(all.begin(), _test);
}

void RangeNode::nodeId(quint64 id)
{
    WorkFlowNode::nodeId(id);
    name(QString("range_%1").arg(id));
}

bool RangeNode::next()
{
    return _text->next();
}

void RangeNode::test(RangeTestNode *p)
{
    _test.reset(p);
}

const SPRangeTestNode RangeNode::test() const
{
    return _test;
}
