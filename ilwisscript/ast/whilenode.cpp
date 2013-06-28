#include "ilwis.h"
#include "astnode.h"
#include "operationnode.h"
#include "expressionnode.h"
#include "whilenode.h"

using namespace Ilwis;

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

bool WhileNode::checkCondition(SymbolTable &symbols, int scope) {

    _condition->evaluate(symbols, scope);
    return _condition->value() == true;
}

bool WhileNode::evaluate(SymbolTable &symbols, int scope) {
    if (_condition.isNull())
        return false;

    if ( !_condition->isValid())
        return false;

    while( checkCondition(symbols, scope)) {
        foreach(QSharedPointer<ASTNode> node, _childeren) {
            if ( node->nodeType() == "breakStatement") {
                node->evaluate(symbols, scope);
                if ( node->value() == false)
                    break;
            } else
                node->evaluate(symbols, scope);
        }
    }
    return true;
}
