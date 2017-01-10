#include <QVariant>
#include "ilwis.h"
#include "astnode.h"
#include "operationnodescript.h"
#include "commandhandler.h"
#include "symboltable.h"

using namespace Ilwis;

OperationNodeScript::OperationNodeScript()
{
}

void OperationNodeScript::setLeftTerm(ASTNode *node)
{
    _leftTerm = QSharedPointer<ASTNode>(node);
}

void OperationNodeScript::addRightTerm(OperationNodeScript::Operators op, ASTNode *node)
{
    RightTerm term;
    term._operator = op;
    term._rightTerm = QSharedPointer<ASTNode>(node);
    _rightTerm.push_back(term);
}

bool OperationNodeScript::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
   bool ok =  _leftTerm->evaluate(symbols, scope, ctx)   ;
   const NodeValue& vleft = _leftTerm->value();
   _value = vleft;
   return ok;


}

bool OperationNodeScript::isValid() const
{
    return ! _leftTerm.isNull();
}

bool OperationNodeScript::handleBinaryCases(int index, const NodeValue& vright, const QString &operation,
                                              const QString& relation, SymbolTable &symbols, ExecutionContext *ctx) {
    if ( index >= vright.size())
        return false;

    QString expr;
    if (       SymbolTable::isDataLink(_value[index]) && SymbolTable::isNumerical(vright[index])){
        expr = QString("%1(%2,%3,%4)").arg(operation).arg(_value.toString(index)).arg(vright.toDouble(index)).arg(relation);
    } else if (SymbolTable::isNumerical(_value[index]) && SymbolTable::isDataLink(vright[index])){
        expr = QString("%1(%2,%3,%4)").arg(operation).arg(_value.toDouble(index)).arg(vright.toString(index)).arg(relation);
    } else if (SymbolTable::isDataLink(_value[index]) && SymbolTable::isDataLink(vright[index])) {
        expr = QString("%1(%2,%3,%4)").arg(operation).arg(_value.toString(index)).arg(vright.toString(index)).arg(relation);
    }

    bool ok = Ilwis::commandhandler()->execute(expr, ctx,symbols);
    if ( !ok || ctx->_results.size() != 1)
        return false;
    _value = {ctx->_results[0], NodeValue::ctID};
    return true;
}

bool OperationNodeScript::handleTableCases(int index, const NodeValue& vright, const QString &operation,
                                              const QString& relation, SymbolTable &symbols, ExecutionContext *ctx) {
    if ( index >= vright.size())
        return false;

    QString expr;
    if ( SymbolTable::isNumerical(vright[index]) && SymbolTable::isDataLink(_value[index])){
        expr = QString("%1(%2,%3,%4,%5)").arg(operation).arg(_value.toString(index)).
                                          arg(additionalInfo(ctx,_value.toString(index))).
                                          arg(vright.toDouble(index)).arg(relation);
    } else if (SymbolTable::isNumerical(_value[index]) && SymbolTable::isDataLink(vright[index])){
        expr = QString("%1(%2,%3,%4, %5)").arg(operation).arg(vright.toString(index)).
                                           arg(additionalInfo(ctx,vright.toString(index))).
                                           arg(_value.toDouble(index)).arg(relation);
    } else if (SymbolTable::isDataLink(_value[index]) && SymbolTable::isDataLink(vright[index])) {
        expr = QString("%1(%2,%3,%4,%5)").arg(operation).arg(vright.toString(index)).arg(_value.toString(index)).arg(relation);
    } else if (SymbolTable::isDataLink(vright[index]) && SymbolTable::isNumerical(_value[index])) {
        expr = QString("%1(%2,%3,%4,%5)").arg(operation).arg(vright.toString(index)).arg(_value.toDouble(index)).arg(relation);
    }

    bool ok = Ilwis::commandhandler()->execute(expr, ctx,symbols);
    if ( !ok || ctx->_results.size() != 1)
        return false;
    _value = {ctx->_results[0], NodeValue::ctID};
    return true;
}

IlwisTypes OperationNodeScript::typesUsed(int index, const NodeValue& vright, SymbolTable &symbols) const {
    if ( index >= vright.size())
        return itUNKNOWN;

    IlwisTypes tp1 = symbols.ilwisType(vright[index], vright.id(index));
    IlwisTypes tp2 = symbols.ilwisType(_value[index], _value.id(index));

    if ( tp1 == itUNKNOWN || tp2 == itUNKNOWN)
        return itUNKNOWN;
    return tp1 | tp2;

}

QString OperationNodeScript::additionalInfo(ExecutionContext *ctx, const QString& key ) const {
    const auto& iter = ctx->_additionalInfo.find(key);
    if ( iter == ctx->_additionalInfo.end())
        return sUNDEF;
    return (*iter).second.toString();
}


