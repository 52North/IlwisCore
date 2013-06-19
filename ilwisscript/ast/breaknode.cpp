#include "ilwis.h"
#include "astnode.h"
#include "breaknode.h"

using namespace Ilwis;

BreakNode::BreakNode()
{
}

QString BreakNode::nodeType() const
{
    return "breakStatement";
}
