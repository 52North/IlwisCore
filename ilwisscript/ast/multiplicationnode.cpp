#include "kernel.h"
#include "errorobject.h"
#include "astnode.h"
#include "operationnode.h"
#include "multiplicationnode.h"
#include "symboltable.h"
#include "commandhandler.h"

using namespace Ilwis;

MultiplicationNode::MultiplicationNode()
{
}

QString MultiplicationNode::nodeType() const
{
    return "mult";
}

bool MultiplicationNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
    if(!OperationNode::evaluate(symbols, scope, ctx))
        return false;

    bool ret = true;
    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope, ctx) ;
        const NodeValue& vright = term._rightTerm->value();
        if ( term._operator == OperationNode::oTIMES ){
            ret = handleTimes(vright, symbols, ctx);
        } else   if ( term._operator == OperationNode::oDIVIDED ){
            ret = handleDiv(vright, symbols, ctx);
        }else   if ( term._operator == OperationNode::oMOD ){
            ret = handleMod(vright, symbols, ctx);
        }

        if (!ret)
            return false;
    }

    return ret;
}

bool MultiplicationNode::handleTimes(const NodeValue& vright, SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var = resolveValue(_value, symbols);
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(var)) {
        _value = {vright.toDouble() * var.toDouble(), NodeValue::ctNumerical};
        return true;
    }

    return handleBinaryCoverageCases(vright, "binarymathraster", "times", symbols, ctx);
}

bool MultiplicationNode::handleDiv(const NodeValue& vright, SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var = resolveValue(_value, symbols);
    if (SymbolTable:: isNumerical(vright) && SymbolTable::isNumerical(var)) {
        if ( vright.toDouble() == 0)
            return false;
        _value = {var.toDouble() /  vright.toDouble(), NodeValue::ctNumerical};
        return true;
    }
    return handleBinaryCoverageCases(vright, "binarymathraster", "divide", symbols, ctx);
}

bool MultiplicationNode::handleMod(const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var = resolveValue(_value, symbols);
    if ( SymbolTable::isIntegerNumerical(vright) && SymbolTable::isIntegerNumerical(var)) {
        bool ok1, ok2;
       _value = {var.toInt(&ok1) %  vright.toInt(&ok2), NodeValue::ctNumerical};
       return ok1 && ok2;
    }
    return handleBinaryCoverageCases(vright, "binarymathraster", "mod", symbols, ctx);
}
