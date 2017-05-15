#include "kernel.h"
#include "ilwisdata.h"
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "operation.h"
#include "compare.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(Compare)


Ilwis::OperationImplementation *Compare::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Compare(metaid, expr);
}

Compare::Compare()
{
}

Compare::Compare(quint64 metaid,const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

template<class T> bool compare(const T& va, LogicalOperator op, const T vb){
    switch (op) {
    case loEQ:
        return va == vb;
    case loNEQ:
        return va != vb;
    case loLESS:
        return va < vb;
    case loLESSEQ:
        return va <= vb;
    case loGREATER:
        return va > vb;
    case loGREATEREQ:
        return va <= vb;
    default:
        return false;
    }
    return false;
}

bool Compare::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    bool result = false;
    if ( _numeric){
        double v1 = _firstValue.toDouble();
        double v2 = _secondValue.toDouble();
        result = compare(v1, _operator, v2);
    }else {
        result = compare(_firstValue, _operator, _secondValue);
    }

    logOperation(_expression);
    ctx->setOutput(symTable, QVariant(result), sUNDEF, itBOOL, Resource());
    return true;
}

OperationImplementation::State Compare::prepare(ExecutionContext *ctx,const SymbolTable&)
{
    if ( _expression.parameterCount() != 3)
        return sPREPAREFAILED;

    _firstValue = _expression.parm(0).value();
    _secondValue = _expression.parm(2).value();
    bool ok1, ok2;
    _firstValue.toDouble(&ok1);
    _secondValue.toDouble(&ok2);
    _numeric = ok1 && ok2;

   QString type = _expression.parm(1).value();
   if ( type == "equals" || type == "=="){
       _operator = loEQ;
   }else if ( type == "not equals" || type == "!="){
       _operator = loNEQ;
   }else if ( type == "greater than" || type == ">"){
       _operator = loGREATER;
   } else if ( type == "less than" || type == "<"){
       _operator = loLESS;
   }else if ( type == "greater or equal" || type == "<="){
       _operator = loGREATEREQ;
   }else if ( type == "less or equal" || type == ">="){
       _operator = loGREATEREQ;
   }

    return sPREPARED;
}


quint64 Compare::createMetadata()
{
    OperationResource operation({"ilwis://operations/compare"});
    operation.setLongName("Compare");
    operation.setSyntax("compare(firstvalue,operator=!equals|notequals|greatherthan|lessthan|greaterorequal|lessorequal,secondvalue)");
    operation.setDescription(TR("Returns set values. compares two values and returns a boolean result"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itNUMBER|itSTRING, TR("first value"),TR("left hand of the compare operation"));
    operation.addInParameter(1,itSTRING, TR("operator"),TR("Logical operator for the comparison"));
    operation.addInParameter(2,itNUMBER|itSTRING, TR("second value"),TR("right hand of the compare method"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itBOOL, TR("result"), TR("Result of the comparison"));
    operation.setKeywords("tests, math,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();
}

