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
        QString pp = vright.toString();
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
    return handleBinaryCoverageCases(vright, "binarymathraster", "add", symbols);
}

bool AddNode::handleSubstract(const NodeValue& vright,SymbolTable &symbols) {
    QVariant var = resolveValue(_value, symbols);
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(var)) {
       _value = {var.toDouble() -  vright.toDouble(), NodeValue::ctNumerical};
       return true;
    }
    return handleBinaryCoverageCases(vright, "binarymathraster", "substract", symbols);
}

