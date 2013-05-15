#include "astnode.h"
#include "operationnode.h"
#include "expressionnode.h"
#include "whilenode.h"

WhileNode::WhileNode()
{
}

void WhileNode::setCondition(ExpressionNode *expr)
{
    _condition = QSharedPointer<ExpressionNode>(expr);
}

QString WhileNode::nodeType() const
{
    return "whileStatement";
}
