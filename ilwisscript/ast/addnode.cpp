#include "kernel.h"
#include "errorobject.h"
#include "astnode.h"
#include "operationnode.h"
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
    if(!OperationNode::evaluate(symbols, scope, ctx))
        return false;

    bool ret = true;
    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope, ctx) ;
        const NodeValue& vright = term._rightTerm->value();
        if ( term._operator == OperationNode::oADD ){
            ret = handleAdd(vright, symbols, ctx);
        } else   if ( term._operator == OperationNode::oSUBSTRACT ){
            ret = handleSubstract(vright, symbols, ctx);
        }
        if (!ret)
            return false;
    }

    return ret;
}

bool AddNode::handleAdd(const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var = resolveValue(_value, symbols);
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(var)) {
       _value = {vright.toDouble() + var.toDouble(), NodeValue::ctNumerical};
       return true;
    }
    IlwisTypes used = typesUsed(vright, symbols);
    bool ok = false;
    if ( hasType(used, itRASTER))
        ok = handleBinaryCoverageCases(vright, "binarymathraster", "add", symbols, ctx);
    else if ( (used & itFEATURE) == 0){
        ok = handleBinaryCoverageCases(vright, "binarymathfeature", "add", symbols, ctx);

    }
    return ok;
}

bool AddNode::handleSubstract(const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var = resolveValue(_value, symbols);
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(var)) {
       _value = {var.toDouble() -  vright.toDouble(), NodeValue::ctNumerical};
       return true;
    }
    IlwisTypes used = typesUsed(vright, symbols);
    bool ok = false;
    if ( hasType(used, itRASTER))
        ok = handleBinaryCoverageCases(vright, "binarymathraster", "substract", symbols, ctx);
    else if (  (used & itFEATURE) == 0){
        ok = handleBinaryCoverageCases(vright, "binarymathfeature", "substract", symbols, ctx);

    }
    return ok;
}



