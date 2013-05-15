#include "astnode.h"
#include "operationnode.h"
#include "expressionnode.h"
#include "symboltable.h"

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
    const QVariant& vleft = _leftTerm->value();
    _value = vleft;
    bool ret  = true;

    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope) ;
        const NodeValue& vright = term._rightTerm->value();
        if ( term._operator == OperationNode::oAND ){
            ret = handleAnd(vright);
        } else   if ( term._operator == OperationNode::oOR ){
            ret = handleOr(vright);
        } else   if ( term._operator == OperationNode::oXOR ){
            ret = handleXor(vright);
        }
        if (!ret)
            return false;
    }

    return ret;
}

bool ExpressionNode::handleAnd(const NodeValue& vright) {
    if ( vright.canConvert(QVariant::Bool) && _value.canConvert(QVariant::Bool)) {
        _value =  {_value.toBool() &&  vright.toBool(), NodeValue::ctBOOLEAN};
        return true;
    } else  if ( SymbolTable::isIntegerNumerical(vright) && SymbolTable::isIntegerNumerical(_value)){
        _value = {_value.toLongLong() & vright.toLongLong(), NodeValue::ctNumerical};
        //_value = value.convert(_value.type());
        return true;
    }
    return false;
}

bool ExpressionNode::handleOr(const NodeValue& vright) {
    if ( vright.canConvert(QVariant::Bool) && _value.canConvert(QVariant::Bool)) {
        _value =  {_value.toBool() ||  vright.toBool(), NodeValue::ctBOOLEAN};
        return true;
    } else  if ( SymbolTable::isIntegerNumerical(vright) && SymbolTable::isIntegerNumerical(_value)){
        _value = {_value.toLongLong() | vright.toLongLong(), NodeValue::ctNumerical};
        //_value = value.convert(_value.type());
        return true;
    }
    return false;
}

bool ExpressionNode::handleXor(const NodeValue& vright) {
    if ( vright.canConvert(QVariant::Bool) && _value.canConvert(QVariant::Bool)) {
        _value =  {_value.toBool() ^  vright.toBool(), NodeValue::ctBOOLEAN};
        return true;
    } else  if ( SymbolTable::isIntegerNumerical(vright) &&SymbolTable::isIntegerNumerical(_value)){
        _value = {_value.toLongLong() ^ vright.toLongLong(), NodeValue::ctNumerical};
        //_value = value.convert(_value.type());
        return true;
    }
    return false;
}
