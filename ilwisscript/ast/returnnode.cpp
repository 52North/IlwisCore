#include "ilwis.h"
#include "astnode.h"
#include "returnnode.h"

using namespace Ilwis;

ReturnNode::ReturnNode()
{
}

QString ReturnNode::nodeType() const
{
    return "returnstatement";
}


