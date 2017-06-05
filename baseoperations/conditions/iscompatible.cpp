#include "kernel.h"
#include "ilwisdata.h"
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "operation.h"
#include "iscompatible.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(IsCompatible)


Ilwis::OperationImplementation *IsCompatible::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new IsCompatible(metaid, expr);
}

IsCompatible::IsCompatible()
{
}

IsCompatible::IsCompatible(quint64 metaid,const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool IsCompatible::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    bool result = _object1->isCompatibleWith(_object2.ptr());

    logOperation(_expression);
    ctx->setOutput(symTable, QVariant(result), sUNDEF, itBOOL, Resource());
    return true;
}

OperationImplementation::State IsCompatible::prepare(ExecutionContext *ctx,const SymbolTable&)
{
    if ( _expression.parameterCount() != 3)
        return sPREPAREFAILED;

    OperationHelper::check([&] ()->bool { return _object1.prepare(_expression.input<QString>(0), itILWISOBJECT); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

    OperationHelper::check([&] ()->bool { return _object2.prepare(_expression.input<QString>(1), itILWISOBJECT); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(1), "" } );


    return sPREPARED;
}


quint64 IsCompatible::createMetadata()
{
    OperationResource operation({"ilwis://operations/iscompatible"});
    operation.setSyntax("iscompatible(firstvalue,secondvalue)");
    operation.setDescription(TR("Checks if two ilwisobjects are compatible with each othert. For example are two georefs compatible/equal"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itILWISOBJECT, TR("first value"),TR("left hand of the IsCompatible operation"));
    operation.addInParameter(1,itILWISOBJECT, TR("second value"),TR("right hand of the IsCompatible method"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itBOOL, TR("result"), TR("Result of the comparison"));
    operation.setKeywords("tests, workflow");

    mastercatalog()->addItems({operation});
    return operation.id();
}

