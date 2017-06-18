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
#include "conditionNode.h"
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

void RangeNode::nodeId(quint64 id)
{
    WorkFlowNode::nodeId(id);
    name(QString("loop_%1").arg(id));
}
