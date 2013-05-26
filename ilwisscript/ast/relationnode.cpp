#include "astnode.h"
#include "operationnode.h"
#include "relationnode.h"
#include "symboltable.h"
#include "commandhandler.h"

RelationNode::RelationNode()
{
}

QString RelationNode::nodeType() const
{
    return "relation";
}

bool RelationNode::evaluate(SymbolTable &symbols, int scope)
{
    if (!OperationNode::evaluate(symbols, scope))
        return false;

    bool ret = true;
    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope) ;
        const NodeValue& vright = term._rightTerm->value();
        if ( term._operator == OperationNode::oEQ ){
            ret = handleEQ(vright);
        } else   if ( term._operator == OperationNode::oGREATER ){
            ret = handleGREATER(vright);
        } else   if ( term._operator == OperationNode::oGREATEREQ ){
            ret = handleGREATEREQ(vright);
        }  else   if ( term._operator == OperationNode::oLESS ){
            ret = handleLESS(vright);
        } else   if ( term._operator == OperationNode::oLESSEQ ){
            ret = handleLESSEQ(vright);
        }  else   if ( term._operator == OperationNode::oNEQ ){
            ret = handleNEQ(vright);
        }
        if(!ret) {
            return ret;
        }
    }
    return ret;
}


bool RelationNode::handleEQ(const NodeValue& vright) {

    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(_value)) {
       _value = {vright.toDouble() == _value.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCoverageCases(vright, "binarylogicalraster", "eq");
}

bool RelationNode::handleNEQ(const NodeValue& vright) {
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(_value)) {
       _value = {vright.toDouble() != _value.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCoverageCases(vright, "binarylogicalraster","neq");
}

bool RelationNode::handleGREATEREQ(const NodeValue& vright) {
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(_value)) {
       _value = {vright.toDouble() >= _value.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCoverageCases(vright, "binarylogicalraster", "greatereq");
}

bool RelationNode::handleGREATER(const NodeValue& vright) {
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(_value)) {
       _value = {vright.toDouble() > _value.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCoverageCases(vright, "binarylogicalraster","greater");
}

bool RelationNode::handleLESS(const NodeValue& vright) {
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(_value)) {
       _value = {vright.toDouble() < _value.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCoverageCases(vright, "binarylogicalraster","less");
}

bool RelationNode::handleLESSEQ(const NodeValue& vright) {
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(_value)) {
       _value = {vright.toDouble() <= _value.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCoverageCases(vright,"binarylogicalraster", "lesseq");
}
