#include "ilwis.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "parametersnode.h"
#include "functionstatementnode.h"

using namespace Ilwis;

FunctionStatementNode::FunctionStatementNode(char *s) : IDNode(s)
{
}

void FunctionStatementNode::setParameters(ParametersNode *parm)
{
    _parameters = QSharedPointer<ParametersNode>(parm);
}

QString FunctionStatementNode::nodeType() const
{
    return "functionStatement";
}
