#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "fcoordinate.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(FCoordinate)

FCoordinate::FCoordinate()
{
}

FCoordinate::FCoordinate(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool FCoordinate::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    if ( !_coord.isValid())
        return false;

    QVariant value;
    value.setValue<Coordinate>(_coord);
    ctx->setOutput(symTable, value, sUNDEF, itCOORDINATE, Resource());

    return true;
}

Ilwis::OperationImplementation *FCoordinate::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new FCoordinate(metaid, expr);
}

Ilwis::OperationImplementation::State FCoordinate::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    bool ok;
    double sx = _expression.parm(0).value().toDouble(&ok);
    if (!ok){
        ERROR2(ERR_ILLEGAL_VALUE_2,"coordinate","x");
        return sPREPAREFAILED;
    }
    double sy = _expression.parm(1).value().toDouble(&ok);
    if (!ok){
        ERROR2(ERR_ILLEGAL_VALUE_2,"coordinate","y");
        return sPREPAREFAILED;
    }
    double sz = 0;
    if (_expression.parameterCount() == 3) {
        sz = _expression.parm(0).value().toDouble(&ok);
        if (!ok){
            ERROR2(ERR_ILLEGAL_VALUE_2,"coordinate","x");
            return sPREPAREFAILED;
        }
    }
    _coord = Coordinate(sx,sy,sz);

    return sPREPARED;
}

quint64 FCoordinate::createMetadata()
{
    OperationResource operation({"ilwis://operations/coordinate"});
    operation.setSyntax("coordinate(x,y[,z])");
    operation.setDescription("translates a x and y value (optional z) into a single coordinate object");
    operation.setInParameterCount({2,3});
    operation.addInParameter(0,itNUMBER, TR("x"));
    operation.addInParameter(1,itNUMBER, TR("y"));
    operation.addOptionalInParameter(2,itNUMBER , TR("z"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itCOORDINATE, TR("coordinate"));
    operation.setKeywords("coordinate,internal,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();
}

