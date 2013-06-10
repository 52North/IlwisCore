#include "symboltable.h"
#include "astnode.h"
#include "operationnode.h"
#include "expressionnode.h"
#include "ifnode.h"

Ifnode::Ifnode() : ASTNode("IfNode")
{
}

bool Ifnode::evaluate(SymbolTable &symbols, int scope)
{
    return false;
}

void Ifnode::setCondition(ExpressionNode *expr)
{
    _condition.reset(expr);
}

void Ifnode::addThen(ASTNode *node)
{
    _then.push_back(QSharedPointer<ASTNode>(node));
}

void Ifnode::addElse(ASTNode *node)
{
     _else.push_back(QSharedPointer<ASTNode>(node));
}
