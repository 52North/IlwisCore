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
#include "loopnode.h"

using namespace Ilwis;

LoopNode::LoopNode()
{

}

Ilwis::WorkFlowNode::NodeTypes Ilwis::LoopNode::type() const
{
    return WorkFlowNode::ntLOOP;
}

bool LoopNode::execute(ExecutionContext *ctx, SymbolTable &symTable, const OperationExpression &expression, const std::map<quint64, int> &idmap)
{
    return false;
}

bool LoopNode::isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck check) const
{
    return WorkFlowCondition::isValid(workflow, check);
}
