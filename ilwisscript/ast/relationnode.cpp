#include "kernel.h"
#include "iooptions.h"
#include "astnode.h"
#include "operationnodescript.h"
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
    if (!OperationNodeScript::evaluate(symbols, scope, ctx))
        return false;

    bool ret = true;
    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope, ctx) ;
        const NodeValue& vright = term._rightTerm->value();
        for(int i=0; i < vright.size(); ++i) {
            if ( term._operator == OperationNodeScript::oEQ ){
                ret = handleEQ(i,vright, symbols, ctx);
            } else   if ( term._operator == OperationNodeScript::oGREATER ){
                ret = handleGREATER(i,vright, symbols, ctx);
            } else   if ( term._operator == OperationNodeScript::oGREATEREQ ){
                ret = handleGREATEREQ(i,vright, symbols, ctx);
            }  else   if ( term._operator == OperationNodeScript::oLESS ){
                ret = handleLESS(i,vright, symbols, ctx);
            } else   if ( term._operator == OperationNodeScript::oLESSEQ ){
                ret = handleLESSEQ(i, vright, symbols, ctx);
            }  else   if ( term._operator == OperationNodeScript::oNEQ ){
                ret = handleNEQ(i,vright, symbols, ctx);
            }
            if(!ret) {
                return ret;
            }
        }
    }
    return ret;
}


bool RelationNode::handleEQ(int index,const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {

    QVariant var1 = resolveValue(index,_value,symbols);
    QVariant var2 = resolveValue(index, vright, symbols);
    if ( ctx->_useAdditionalParameters){
        return handleImplicitCase(var1, var2, ctx, "==")    ;
    }
    else if ( SymbolTable::isNumerical(var2) && SymbolTable::isNumerical(var1)) {
       _value = {var1.toDouble() == var2.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    if ( SymbolTable::isString(var2) && SymbolTable::isString(var1)) {
       _value = {var1.toString() == var2.toString(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCases(index, vright, "binarylogicalraster", "eq", symbols, ctx);
}

bool RelationNode::handleNEQ(int index,const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var1 = resolveValue(index, _value,symbols);
    QVariant var2 = resolveValue(index, vright, symbols);
    if ( ctx->_useAdditionalParameters){
        return handleImplicitCase(var1, var2, ctx, "!=")    ;
    }
    else if ( SymbolTable::isNumerical(var2) && SymbolTable::isNumerical(var1)) {
       _value = {var1.toDouble() != var2.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    if ( SymbolTable::isString(var2) && SymbolTable::isString(var1)) {
       _value = {var1.toString() != var2.toString(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCases(index, vright, "binarylogicalraster","neq", symbols, ctx);
}

bool RelationNode::handleGREATEREQ(int index,const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var1 = resolveValue(index, _value,symbols);
    QVariant var2 = resolveValue(index, vright, symbols);
    if ( ctx->_useAdditionalParameters){
        return handleImplicitCase(var1, var2, ctx, ">=")    ;
    }
    else if ( SymbolTable::isNumerical(var2) && SymbolTable::isNumerical(var1)) {
       _value = {var1.toDouble() >= var2.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCases(index, vright, "binarylogicalraster", "greatereq", symbols, ctx);
}

bool RelationNode::handleGREATER(int index,const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var1 = resolveValue(index, _value,symbols);
    QVariant var2 = resolveValue(index,vright, symbols);
    if ( ctx->_useAdditionalParameters){
        return handleImplicitCase(var1, var2, ctx, ">")    ;
    }
    else if ( SymbolTable::isNumerical(var2) && SymbolTable::isNumerical(var1)) {
        _value = { var1.toDouble() > var2.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCases(index, vright, "binarylogicalraster","greater", symbols, ctx);
}

bool RelationNode::handleLESS(int index,const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var1 = resolveValue(index, _value,symbols);
    QVariant var2 = resolveValue(index, vright, symbols);
    if ( ctx->_useAdditionalParameters){
        return handleImplicitCase(var1, var2, ctx, "<")    ;
    }
    else if ( SymbolTable::isNumerical(var1) && SymbolTable::isNumerical(var2)) {
       _value = { var1.toDouble() < var2.toDouble() , NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCases(index, vright, "binarylogicalraster","less", symbols, ctx);
}

bool RelationNode::handleLESSEQ(int index,const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var1 = resolveValue(index, _value,symbols);
    QVariant var2 = resolveValue(index, vright, symbols);
    if ( ctx->_useAdditionalParameters){
        return handleImplicitCase(var1, var2, ctx, "<=")    ;
    }
    else if ( SymbolTable::isNumerical(var2) && SymbolTable::isNumerical(var1)) {
        _value = {var1.toDouble() <= var2.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCases(index, vright,"binarylogicalraster", "lesseq",symbols, ctx);
}

bool RelationNode::handleImplicitCase(const QVariant& var1, const QVariant& var2,  ExecutionContext *ctx, const QString& oper){
    QString inname = ctx->_additionalInfo[IMPLICITPARMATER0].toString();
    QString outname = ANONYMOUS_PREFIX;
    QString expression = QString("%1=select(%2,\"attribute=%3%4%5\",asIndex)").arg(outname).arg(inname).arg(var1.toString()).arg(oper).arg(var2.toString());
    SymbolTable symbols;
    int lastResult = ctx->_results.size();
    if(!Ilwis::commandhandler()->execute(expression, ctx, symbols)) {
        ERROR0(TR("Expression execution error in script; script aborted. See log for further details"));
        return false;
    }
    Indices index = symbols.getValue<Indices>(ctx->_results[lastResult]);
    QVariant value;
    value.setValue<Indices>(index);
    _value = {value, NodeValue::ctLIST};
    return true;
}
