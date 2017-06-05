#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "numericoperation.h"
#include "numbercondition.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(NumberCondition)

Ilwis::OperationImplementation *NumberCondition::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new NumberCondition(metaid, expr);
}

NumberCondition::NumberCondition()
{
}

NumberCondition::NumberCondition(quint64 metaid,const Ilwis::OperationExpression &expr) : NumericOperation(metaid, expr)
{
}

bool NumberCondition::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    //Execute

    bool conditionIsTrue = false;

    switch(_condition){
    case 0: //Greater than
        conditionIsTrue = _firstValue > _secondValue;
        break;
    case 1: //Smaller than
        conditionIsTrue = _firstValue < _secondValue;
        break;
    case 2: //Greater or equals
        conditionIsTrue = _firstValue >= _secondValue;
        break;
    case 3: //Smaller or equals
        conditionIsTrue = _firstValue <= _secondValue;
        break;
    case 4: //Equals
        conditionIsTrue = _firstValue == _secondValue;
        break;
    case 5: //Not equals
        conditionIsTrue = _firstValue != _secondValue;
        break;
    }

    ctx->addOutput(symTable, conditionIsTrue, sUNDEF, itBOOL, Resource());

    return true;
}

OperationImplementation::State NumberCondition::prepare(ExecutionContext *ctx,const SymbolTable&)
{
    if ( _expression.parameterCount() != 3){
        return sPREPAREFAILED;
    }

    if (!hasType(_expression.parm(0).valuetype(),itNUMBER) || !hasType(_expression.parm(2).valuetype(),itNUMBER))
    {
        return sPREPAREFAILED;
    }

    _firstValue = _expression.parm(0).value().toDouble();
    QString parmOperation = _expression.parm(1).value();
    _secondValue = _expression.parm(2).value().toDouble();

    if(parmOperation == "greater than") {
        _condition = 0;
    }
    else if (parmOperation == "smaller than") {
        _condition = 1;
    }
    else if (parmOperation == "greater or equals") {
        _condition = 2;
    }
    else if (parmOperation == "smaller or equals") {
        _condition = 3;
    }
    else if (parmOperation == "equals") {
        _condition = 4;
    }
    else if (parmOperation == "not equals") {
        _condition = 5;
    }
    else {
        return sPREPAREFAILED;
    }

    return sPREPARED;
}


quint64 NumberCondition::createMetadata()
{
    OperationResource operation({"ilwis://operations/numbercondition"});
    operation.setLongName("conditions to compare/check integers");
    operation.setSyntax("numbercondition(FirstValue, Condition=!greater than|smaller than|greater or equals|smaller or equals|equals|not equals, SecondValue)");
    operation.setDescription(TR("generates a rastercoverage where the atmospheric errors are corrected"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itNUMBER, TR("first number value"),TR("the first value used in the condition"));
    operation.addInParameter(1,itSTRING, TR("the condition"),TR("the condition which has to be checked. 'greater than', 'equals', 'not equals'"));
    operation.addInParameter(2,itNUMBER, TR("second number value"),TR("the first value used in the condition"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itBOOL, TR("valid"), TR("true if condition is met"));
    operation.setKeywords("condition, numeric, internal");

    mastercatalog()->addItems({operation});
    return operation.id();
}
