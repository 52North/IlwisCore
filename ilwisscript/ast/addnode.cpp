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

bool AddNode::evaluate(SymbolTable &symbols, int scope)
{
    if(!OperationNode::evaluate(symbols, scope))
        return false;

    bool ret = true;
    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope) ;
        const NodeValue& vright = term._rightTerm->value();
        if ( term._operator == OperationNode::oADD ){
            ret = handleAdd(vright, symbols);
        } else   if ( term._operator == OperationNode::oSUBSTRACT ){
            ret = handleSubstract(vright, symbols);
        }
        if (!ret)
            return false;
    }

    return ret;
}

bool AddNode::handleAdd(const NodeValue& vright,SymbolTable &symbols) {
    QVariant var = resolveValue(_value, symbols);
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(var)) {
       _value = {vright.toDouble() + var.toDouble(), NodeValue::ctNumerical};
       return true;
    }
    IlwisTypes used = typesUsed(vright, symbols);
    bool ok = false;
    if ( hasType(used, itRASTER))
        ok = handleBinaryCoverageCases(vright, "binarymathraster", "add", symbols);
    else if ( (used & itFEATURE) == 0){
        ok = handleBinaryCoverageCases(vright, "binarymathfeature", "add", symbols);

    }
    return ok;
}

bool AddNode::handleSubstract(const NodeValue& vright,SymbolTable &symbols) {
    QVariant var = resolveValue(_value, symbols);
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(var)) {
       _value = {var.toDouble() -  vright.toDouble(), NodeValue::ctNumerical};
       return true;
    }
    IlwisTypes used = typesUsed(vright, symbols);
    bool ok = false;
    if ( hasType(used, itRASTER))
        ok = handleBinaryCoverageCases(vright, "binarymathraster", "substract", symbols);
    else if (  (used & itFEATURE) == 0){
        ok = handleBinaryCoverageCases(vright, "binarymathfeature", "substract", symbols);

    }
    return ok;
}

IlwisTypes AddNode::typesUsed(const NodeValue& vright, SymbolTable &symbols) const {
    IlwisTypes tp1 = symbols.ilwisType(vright.id());
    IlwisTypes tp2 = symbols.ilwisType(_value.id());

    if ( tp1 == itUNKNOWN || tp2 == itUNKNOWN)
        return itUNKNOWN;
    return tp1 | tp2;

}

