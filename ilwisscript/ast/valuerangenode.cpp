#include "ilwis.h"
#include "astnode.h"
#include "valuerangenode.h"

using namespace Ilwis;

ValueRangeNode::ValueRangeNode()
{
}

QString ValueRangeNode::nodeType() const
{
    return "valrangePart";
}
