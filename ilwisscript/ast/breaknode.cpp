#include "astnode.h"
#include "breaknode.h"

BreakNode::BreakNode()
{
}

QString BreakNode::nodeType() const
{
    return "breakStatement";
}
