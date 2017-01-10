#include <QVariant>
#include <QRegExp>
#include "kernel.h"
#include "catalog.h"
#include "ilwisobject.h"
#include "astnode.h"
#include "idnode.h"
#include "operationnodescript.h"
#include "symboltable.h"
#include "expressionnode.h"
#include "variablenode.h"

using namespace Ilwis;

VariableNode::VariableNode(bool constant) : IDNode(0), _constant(constant)
{
}


QString VariableNode::nodeType() const
{
    return "Variable";
}

void VariableNode::setExpression(ExpressionNode * node)
{
    _expression = QSharedPointer<ExpressionNode>(node)    ;
}

bool VariableNode::evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx)
{
    QVariant var;
    if ( !_expression.isNull()) {
        _expression->evaluate(symbols, scope, ctx);
        var = _expression->value();
    }


    foreach(QSharedPointer<ASTNode> node, _childeren) {
        if ( node->nodeType() == "ID") {
            IDNode *idnode = static_cast<IDNode *>( node.data());
            idnode->evaluate(symbols, scope, ctx);
            IlwisTypes type = idnode->type();
            symbols.addSymbol(idnode->id(), scope,type, var);
        }
    }
    return true;
}


