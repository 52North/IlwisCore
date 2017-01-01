#include <QVariant>
#include <set>
#include "ilwis.h"
#include "identity.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "operationnodescript.h"
#include "expressionnode.h"
#include "variablenode.h"
#include "returnnode.h"
#include "functionnode.h"

using namespace Ilwis;

FunctionNode::FunctionNode(char *s) : IDNode(s)
{
}

void FunctionNode::setId(IDNode *node)
{
    _id = QSharedPointer<IDNode>(node);
}

void FunctionNode::addParameter(VariableNode *node)
{
    _parameters.push_back(QSharedPointer<VariableNode>(node));
}

void FunctionNode::setReturn(ReturnNode *node)
{
    _return = QSharedPointer<ReturnNode>(node);
}

QString FunctionNode::nodeType() const
{
    return "function";
}
