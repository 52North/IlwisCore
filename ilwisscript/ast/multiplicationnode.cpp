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

bool MultiplicationNode::evaluate(SymbolTable &symbols, int scope)
{
    if(!OperationNode::evaluate(symbols, scope))
        return false;

    bool ret = true;
    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope) ;
        const NodeValue& vright = term._rightTerm->value();
        if ( term._operator == OperationNode::oTIMES ){
            ret = handleTimes(vright);
        } else   if ( term._operator == OperationNode::oDIVIDED ){
            ret = handleDiv(vright);
        }else   if ( term._operator == OperationNode::oMOD ){
            ret = handleMod(vright);
        }

        if (!ret)
            return false;
    }

    return ret;
}

bool MultiplicationNode::handleTimes(const NodeValue& vright) {
    QString expr;
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(_value)) {
        _value = {vright.toDouble() * _value.toDouble(), NodeValue::ctNumerical};
        return true;
    }

    return handleBinaryCoverageCases(vright, "binarymathraster", "times");
}

bool MultiplicationNode::handleDiv(const NodeValue& vright) {
    QString expr;
    if (SymbolTable:: isNumerical(vright) && SymbolTable::isNumerical(_value)) {
        if ( vright.toDouble() == 0)
            return false;
        _value = {_value.toDouble() /  vright.toDouble(), NodeValue::ctNumerical};
        return true;
    }
    return handleBinaryCoverageCases(vright, "binarymathraster", "divide");
}

bool MultiplicationNode::handleMod(const NodeValue& vright) {
    if ( SymbolTable::isIntegerNumerical(vright) && SymbolTable::isIntegerNumerical(_value)) {
        bool ok1, ok2;
       _value = {_value.toInt(&ok1) %  vright.toInt(&ok2), NodeValue::ctNumerical};
       return ok1 && ok2;
    }
    return handleBinaryCoverageCases(vright, "binarymathraster", "mod");
}
