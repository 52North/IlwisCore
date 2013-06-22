#include "ilwis.h"
#include "astnode.h"
#include "operationnode.h"
#include "expressionnode.h"
#include "symboltable.h"

using namespace Ilwis;

ExpressionNode::ExpressionNode()
{
}

QString ExpressionNode::nodeType() const
{
    return "expression";
}

bool ExpressionNode::evaluate(SymbolTable &symbols, int scope)
{
    OperationNode::evaluate(symbols, scope);
    const NodeValue& vleft = _leftTerm->value();
    _value = vleft;
    bool ret  = true;

    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope) ;
        const NodeValue& vright = term._rightTerm->value();
        if ( term._operator == OperationNode::oAND ){
            ret = handleAnd(vright, symbols);
        } else   if ( term._operator == OperationNode::oOR ){
            ret = handleOr(vright, symbols);
        } else   if ( term._operator == OperationNode::oXOR ){
            ret = handleXor(vright, symbols);
        }
        if (!ret)
            return false;
    }

    return ret;
}

bool ExpressionNode::handleAnd(const NodeValue& vright,SymbolTable &symbols) {
    if ( vright.canConvert(QVariant::Bool) && _value.canConvert(QVariant::Bool)) {
        _value =  {_value.toBool() &&  vright.toBool(), NodeValue::ctBOOLEAN};
        return true;
    } else  if ( SymbolTable::isIntegerNumerical(vright) && SymbolTable::isIntegerNumerical(_value)){
        _value = {_value.toLongLong() & vright.toLongLong(), NodeValue::ctNumerical};
        return true;
    }
   return handleBinaryCoverageCases(vright, "binarylogicalraster", "and", symbols);
}

bool ExpressionNode::handleOr(const NodeValue& vright,SymbolTable &symbols) {
    if ( vright.canConvert(QVariant::Bool) && _value.canConvert(QVariant::Bool)) {
        _value =  {_value.toBool() ||  vright.toBool(), NodeValue::ctBOOLEAN};
        return true;
    } else  if ( SymbolTable::isIntegerNumerical(vright) && SymbolTable::isIntegerNumerical(_value)){
        _value = {_value.toLongLong() | vright.toLongLong(), NodeValue::ctNumerical};
        return true;
    }
    return handleBinaryCoverageCases(vright, "binarylogicalraster", "or", symbols);
}

bool ExpressionNode::handleXor(const NodeValue& vright,SymbolTable &symbols) {
    if ( vright.canConvert(QVariant::Bool) && _value.canConvert(QVariant::Bool)) {
        _value =  {_value.toBool() ^  vright.toBool(), NodeValue::ctBOOLEAN};
        return true;
    } else  if ( SymbolTable::isIntegerNumerical(vright) &&SymbolTable::isIntegerNumerical(_value)){
        _value = {_value.toLongLong() ^ vright.toLongLong(), NodeValue::ctNumerical};
        return true;
    }
    return handleBinaryCoverageCases(vright, "binarylogicalraster", "xor", symbols);
}
