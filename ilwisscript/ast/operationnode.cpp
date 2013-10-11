#include <QVariant>
#include "ilwis.h"
#include "astnode.h"
#include "operationnode.h"
#include "commandhandler.h"
#include "symboltable.h"

using namespace Ilwis;

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

bool OperationNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
   bool ok =  _leftTerm->evaluate(symbols, scope, ctx)   ;
   const NodeValue& vleft = _leftTerm->value();
   _value = vleft;
   return ok;


}

bool OperationNode::isValid() const
{
    return ! _leftTerm.isNull();
}

bool OperationNode::handleBinaryCoverageCases(const NodeValue& vright, const QString &operation,
                                              const QString& relation,SymbolTable &symbols, ExecutionContext *ctx) {
    QString expr;
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isDataLink(_value)){
        expr = QString("%1(%2,%3,%4)").arg(operation).arg(_value.toString()).arg(vright.toDouble()).arg(relation);
    } else if (SymbolTable::isNumerical(_value) && SymbolTable::isDataLink(vright)){
        expr = QString("%1(%2,%3,%4)").arg(operation).arg(vright.toString()).arg(_value.toDouble()).arg(relation);
    } else if (SymbolTable::isDataLink(_value) && SymbolTable::isDataLink(vright)) {
        expr = QString("%1(%2,%3,%4)").arg(operation).arg(vright.toString()).arg(_value.toString()).arg(relation);
    } else if (SymbolTable::isDataLink(vright) && SymbolTable::isNumerical(_value)) {
        expr = QString("%1(%2,%3,%4)").arg(operation).arg(vright.toString()).arg(_value.toDouble()).arg(relation);
    }

    bool ok = Ilwis::commandhandler()->execute(expr, ctx,symbols);
    if ( !ok || ctx->_results.size() != 1)
        return false;
    _value = {ctx->_results[0], NodeValue::ctID};
    return true;
}

IlwisTypes OperationNode::typesUsed(const NodeValue& vright, SymbolTable &symbols) const {
    IlwisTypes tp1 = symbols.ilwisType(vright.id());
    IlwisTypes tp2 = symbols.ilwisType(_value.id());

    if ( tp1 == itUNKNOWN || tp2 == itUNKNOWN)
        return itUNKNOWN;
    return tp1 | tp2;

}


