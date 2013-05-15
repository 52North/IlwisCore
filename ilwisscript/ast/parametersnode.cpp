#include "astnode.h"
#include "operationnode.h"
#include "expressionnode.h"
#include "parametersnode.h"

ParametersNode::ParametersNode()
{
}

QString ParametersNode::nodeType() const
{
    return "actualParameters";
}
