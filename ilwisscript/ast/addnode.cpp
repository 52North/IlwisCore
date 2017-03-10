#include "kernel.h"
#include "errorobject.h"
#include "astnode.h"
#include "operationnodescript.h"
#include "addnode.h"
#include "symboltable.h"
#include "commandhandler.h"

using namespace Ilwis;

AddNode::AddNode()
{
}

QString AddNode::nodeType() const
{
    return "add";
}

bool AddNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
    if(!OperationNodeScript::evaluate(symbols, scope, ctx))
        return false;

    bool ret = true;
    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope, ctx) ;
        const NodeValue& vright = term._rightTerm->value();
        for(int i=0; i < vright.size(); ++i) {
            if ( term._operator == OperationNodeScript::oADD ){
                ret = handleAdd(i, vright, symbols, ctx);
            } else   if ( term._operator == OperationNodeScript::oSUBSTRACT ){
                ret = handleSubstract(i, vright, symbols, ctx);
            }
            if (!ret)
                return false;
        }
    }

    return ret;
}

bool AddNode::handleAdd(int index, const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var = resolveValue(index, _value, symbols);
    if ( SymbolTable::isNumerical(vright[index]) && SymbolTable::isNumerical(var)) {
       _value = {vright.toDouble(index) + var.toDouble(), NodeValue::ctNumerical};
       return true;
    }
    IlwisTypes used = typesUsed(index, vright, symbols);
    bool ok = false;
    if ( hasType(used, itRASTER))
        ok = handleBinaryCases(index, vright, "binarymathraster", "add", symbols, ctx);
    else if ( hasType(used , itFEATURE)){
        ok = handleBinaryCases(index, vright, "binarymathfeature", "add", symbols, ctx);

    } else if ( hasType(used,itTABLE)){
        ok = handleTableCases(index, vright, "binarymathtable", "add", symbols, ctx);

    }
    return ok;
}

bool AddNode::handleSubstract(int index, const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var = resolveValue(index, _value, symbols);
    if ( SymbolTable::isNumerical(vright[index]) && SymbolTable::isNumerical(var)) {
       _value = {var.toDouble() -  vright.toDouble(index), NodeValue::ctNumerical};
       return true;
    }
    IlwisTypes used = typesUsed(index, vright, symbols);
    bool ok = false;
    if ( hasType(used, itRASTER))
        ok = handleBinaryCases(index, vright, "binarymathraster", "subtract", symbols, ctx);
    else if (  (used & itFEATURE) == 0){
        ok = handleBinaryCases(index, vright, "binarymathfeature", "subtract", symbols, ctx);

    }
    return ok;
}



