#include "astnode.h"
#include "valuerangenode.h"

ValueRangeNode::ValueRangeNode()
{
}

QString ValueRangeNode::nodeType() const
{
    return "valrangePart";
}
