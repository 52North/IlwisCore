#include <QVariant>
#include "astnode.h"
#include "operationnode.h"
#include "commandhandler.h"
#include "symboltable.h"

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

bool OperationNode::handleBinaryCoverageCases(const NodeValue& vright, const QString &operation, const QString& relation) {
    QString expr;
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isDataLink(_value)){
        expr = QString("%1(%2,%3,%4)").arg(operation).arg(_value.toString()).arg(vright.toDouble()).arg(relation);
    } else if (SymbolTable::isNumerical(_value) && SymbolTable::isDataLink(vright)){
        expr = QString("%1(%2,%3,%4)").arg(operation).arg(vright.toDouble()).arg(_value.toString()).arg(relation);
    } else if (SymbolTable::isDataLink(_value) && SymbolTable::isDataLink(vright)) {
        expr = QString("%1(%2,%3,%4)").arg(operation).arg(vright.toString()).arg(_value.toString()).arg(relation);
    }
    Ilwis::ExecutionContext ctx;
    bool ok = Ilwis::commandhandler()->execute(expr, &ctx);
    if ( !ok || ctx._results.size() != 1)
        return false;
    _value = {ctx._results[0], NodeValue::ctMethod};
    return true;
}


