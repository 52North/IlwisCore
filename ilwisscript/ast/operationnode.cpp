#include <QVariant>
#include "astnode.h"
#include "operationnode.h"

OperationNode::OperationNode()
{
}

void OperationNode::setLeftTerm(ASTNode *node)
{
    _leftTerm = QSharedPointer<ASTNode>(node);
}

void OperationNode::addRightTerm(OperationNode::Operators op, ASTNode *node)
{
    RightTerm term;
    term._operator = op;
    term._rightTerm = QSharedPointer<ASTNode>(node);
    _rightTerm.push_back(term);
}

bool OperationNode::evaluate(SymbolTable &symbols, int scope)
{
   bool ok =  _leftTerm->evaluate(symbols, scope)   ;
   const NodeValue& vleft = _leftTerm->value();
   _value = vleft;
   return ok;


}

bool OperationNode::isValid() const
{
    return ! _leftTerm.isNull();
}


