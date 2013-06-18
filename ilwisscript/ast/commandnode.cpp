#include <QHash>
#include "astnode.h"
#include "operationnode.h"
#include "expressionnode.h"
#include "commandnode.h"

using namespace Ilwis;

CommandNode::CommandNode()
{
}

QString CommandNode::nodeType() const
{
    return "commandStatement";
}

void CommandNode::setCommand(const QString &com)
{
    _command = com;
}

void CommandNode::addOption(const QString &flag, ExpressionNode *expr)
{
    _options[flag] = QSharedPointer<ExpressionNode>(expr);
}
