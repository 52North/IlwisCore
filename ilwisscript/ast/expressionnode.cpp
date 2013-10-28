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

bool ExpressionNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
    OperationNode::evaluate(symbols, scope, ctx);
    const NodeValue& vleft = _leftTerm->value();
    _value = vleft;
    bool ret  = true;

    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope, ctx) ;
        const NodeValue& vright = term._rightTerm->value();
        for(int i=0; i < vright.size(); ++i) {
            if ( term._operator == OperationNode::oAND ){
                ret = handleAnd(i,vright, symbols, ctx);
            } else   if ( term._operator == OperationNode::oOR ){
                ret = handleOr(i,vright, symbols, ctx);
            } else   if ( term._operator == OperationNode::oXOR ){
                ret = handleXor(i,vright, symbols, ctx);
            }
            if (!ret)
                return false;
        }
    }

    return ret;
}

bool ExpressionNode::handleAnd(int index, const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    if ( vright.canConvert(index,QVariant::Bool) && _value.canConvert(index,QVariant::Bool)) {
        _value =  {_value.toBool(index) &&  vright.toBool(index), NodeValue::ctBOOLEAN};
        return true;
    } else  if ( SymbolTable::isIntegerNumerical(vright) && SymbolTable::isIntegerNumerical(_value)){
        _value = {_value.toLongLong(index) & vright.toLongLong(index), NodeValue::ctNumerical};
        return true;
    }
   return handleBinaryCases(index, vright, "binarylogicalraster", "and", symbols, ctx);
}

bool ExpressionNode::handleOr(int index,const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    if ( vright.canConvert(index,QVariant::Bool) && _value.canConvert(index,QVariant::Bool)) {
        _value =  {_value.toBool(index) ||  vright.toBool(index), NodeValue::ctBOOLEAN};
        return true;
    } else  if ( SymbolTable::isIntegerNumerical(vright) && SymbolTable::isIntegerNumerical(_value)){
        _value = {_value.toLongLong(index) | vright.toLongLong(index), NodeValue::ctNumerical};
        return true;
    }
    return handleBinaryCases(index, vright, "binarylogicalraster", "or", symbols, ctx);
}

bool ExpressionNode::handleXor(int index,const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    if ( vright.canConvert(index,QVariant::Bool) && _value.canConvert(index,QVariant::Bool)) {
        _value =  {_value.toBool(index) ^  vright.toBool(index), NodeValue::ctBOOLEAN};
        return true;
    } else  if ( SymbolTable::isIntegerNumerical(vright) &&SymbolTable::isIntegerNumerical(_value)){
        _value = {_value.toLongLong(index) ^ vright.toLongLong(index), NodeValue::ctNumerical};
        return true;
    }
    return handleBinaryCases(index,vright, "binarylogicalraster", "xor", symbols, ctx);
}
