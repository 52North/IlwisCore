#include "ilwis.h"
#include "astnode.h"
#include "operationnode.h"
#include "relationnode.h"
#include "symboltable.h"
#include "commandhandler.h"

using namespace Ilwis;

RelationNode::RelationNode()
{
}

QString RelationNode::nodeType() const
{
    return "relation";
}

bool RelationNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
    if (!OperationNode::evaluate(symbols, scope, ctx))
        return false;

    bool ret = true;
    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope, ctx) ;
        const NodeValue& vright = term._rightTerm->value();
        if ( term._operator == OperationNode::oEQ ){
            ret = handleEQ(vright, symbols, ctx);
        } else   if ( term._operator == OperationNode::oGREATER ){
            ret = handleGREATER(vright, symbols, ctx);
        } else   if ( term._operator == OperationNode::oGREATEREQ ){
            ret = handleGREATEREQ(vright, symbols, ctx);
        }  else   if ( term._operator == OperationNode::oLESS ){
            ret = handleLESS(vright, symbols, ctx);
        } else   if ( term._operator == OperationNode::oLESSEQ ){
            ret = handleLESSEQ(vright, symbols, ctx);
        }  else   if ( term._operator == OperationNode::oNEQ ){
            ret = handleNEQ(vright, symbols, ctx);
        }
        if(!ret) {
            return ret;
        }
    }
    return ret;
}


bool RelationNode::handleEQ(const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {

    QVariant var1 = resolveValue(_value,symbols);
    QVariant var2 = resolveValue(vright, symbols);
    if ( SymbolTable::isNumerical(var2) && SymbolTable::isNumerical(var1)) {
       _value = {var1.toDouble() == var2.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    if ( SymbolTable::isString(var2) && SymbolTable::isString(var1)) {
       _value = {var1.toString() == var2.toString(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCases(vright, "binarylogicalraster", "eq", symbols, ctx);
}

bool RelationNode::handleNEQ(const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var1 = resolveValue(_value,symbols);
    QVariant var2 = resolveValue(vright, symbols);
    if ( SymbolTable::isNumerical(var2) && SymbolTable::isNumerical(var1)) {
       _value = {var1.toDouble() != var2.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    if ( SymbolTable::isString(var2) && SymbolTable::isString(var1)) {
       _value = {var1.toString() != var2.toString(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCases(vright, "binarylogicalraster","neq", symbols, ctx);
}

bool RelationNode::handleGREATEREQ(const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var1 = resolveValue(_value,symbols);
    QVariant var2 = resolveValue(vright, symbols);
    if ( SymbolTable::isNumerical(var2) && SymbolTable::isNumerical(var1)) {
       _value = {var1.toDouble() >= var2.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCases(vright, "binarylogicalraster", "greatereq", symbols, ctx);
}

bool RelationNode::handleGREATER(const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var1 = resolveValue(_value,symbols);
    QVariant var2 = resolveValue(vright, symbols);
    if ( SymbolTable::isNumerical(var2) && SymbolTable::isNumerical(var1)) {
        _value = { var1.toDouble() > var2.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCases(vright, "binarylogicalraster","greater", symbols, ctx);
}

bool RelationNode::handleLESS(const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var1 = resolveValue(_value,symbols);
    QVariant var2 = resolveValue(vright, symbols);
    if ( SymbolTable::isNumerical(var1) && SymbolTable::isNumerical(var2)) {
       _value = { var1.toDouble() < var2.toDouble() , NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCases(vright, "binarylogicalraster","less", symbols, ctx);
}

bool RelationNode::handleLESSEQ(const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var1 = resolveValue(_value,symbols);
    QVariant var2 = resolveValue(vright, symbols);
    if ( SymbolTable::isNumerical(var2) && SymbolTable::isNumerical(var1)) {
        _value = {var1.toDouble() <= var2.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCases(vright,"binarylogicalraster", "lesseq",symbols, ctx);
}
