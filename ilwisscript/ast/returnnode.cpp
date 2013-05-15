#include "astnode.h"
#include "returnnode.h"

ReturnNode::ReturnNode()
{
}

QString ReturnNode::nodeType() const
{
    return "returnstatement";
}


