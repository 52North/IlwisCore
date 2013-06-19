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

bool RelationNode::evaluate(SymbolTable &symbols, int scope)
{
    if (!OperationNode::evaluate(symbols, scope))
        return false;

    bool ret = true;
    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope) ;
        const NodeValue& vright = term._rightTerm->value();
        if ( term._operator == OperationNode::oEQ ){
            ret = handleEQ(vright, symbols);
        } else   if ( term._operator == OperationNode::oGREATER ){
            ret = handleGREATER(vright, symbols);
        } else   if ( term._operator == OperationNode::oGREATEREQ ){
            ret = handleGREATEREQ(vright, symbols);
        }  else   if ( term._operator == OperationNode::oLESS ){
            ret = handleLESS(vright, symbols);
        } else   if ( term._operator == OperationNode::oLESSEQ ){
            ret = handleLESSEQ(vright, symbols);
        }  else   if ( term._operator == OperationNode::oNEQ ){
            ret = handleNEQ(vright, symbols);
        }
        if(!ret) {
            return ret;
        }
    }
    return ret;
}


bool RelationNode::handleEQ(const NodeValue& vright,SymbolTable &symbols) {

    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(_value)) {
       _value = {vright.toDouble() == _value.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCoverageCases(vright, "binarylogicalraster", "eq", symbols);
}

bool RelationNode::handleNEQ(const NodeValue& vright,SymbolTable &symbols) {
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(_value)) {
       _value = {vright.toDouble() != _value.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCoverageCases(vright, "binarylogicalraster","neq", symbols);
}

bool RelationNode::handleGREATEREQ(const NodeValue& vright,SymbolTable &symbols) {
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(_value)) {
       _value = {vright.toDouble() >= _value.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCoverageCases(vright, "binarylogicalraster", "greatereq", symbols);
}

bool RelationNode::handleGREATER(const NodeValue& vright,SymbolTable &symbols) {
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(_value)) {
       _value = {vright.toDouble() > _value.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCoverageCases(vright, "binarylogicalraster","greater", symbols);
}

bool RelationNode::handleLESS(const NodeValue& vright,SymbolTable &symbols) {
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(_value)) {
       _value = {vright.toDouble() < _value.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCoverageCases(vright, "binarylogicalraster","less", symbols);
}

bool RelationNode::handleLESSEQ(const NodeValue& vright,SymbolTable &symbols) {
    if ( SymbolTable::isNumerical(vright) && SymbolTable::isNumerical(_value)) {
       _value = {vright.toDouble() <= _value.toDouble(), NodeValue::ctBOOLEAN};
       return true;
    }
    return handleBinaryCoverageCases(vright,"binarylogicalraster", "lesseq",symbols);
}
