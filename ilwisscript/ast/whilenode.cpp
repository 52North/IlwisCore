#include <set>
#include "ilwis.h"
#include "identity.h"
#include "astnode.h"
#include "operationnodescript.h"
#include "symboltable.h"
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

bool WhileNode::checkCondition(SymbolTable &symbols, int scope, ExecutionContext *ctx) {

    _condition->evaluate(symbols, scope, ctx);
    return _condition->value().toBool(0) == true;
}

bool WhileNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx) {
    if (_condition.isNull())
        return false;

    if ( !_condition->isValid())
        return false;

    while( checkCondition(symbols, scope, ctx)) {
        foreach(QSharedPointer<ASTNode> node, _childeren) {
            if ( node->nodeType() == "breakStatement") {
                node->evaluate(symbols, scope, ctx);
                if ( node->value().toBool(0) == false)
                    break;
            } else
                node->evaluate(symbols, scope, ctx);
        }
    }
    return true;
}
