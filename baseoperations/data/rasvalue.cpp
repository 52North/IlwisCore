#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "rasvalue.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(RasValue)

RasValue::RasValue()
{
}

RasValue::RasValue(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool RasValue::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    double v = _inputGC->pix2value(_pix);
    if ( ctx) {
        logOperation(_expression);
        ctx->setOutput(symTable, QVariant(v), sUNDEF, itDOUBLE, Resource());
    }
    return true;
}

Ilwis::OperationImplementation *RasValue::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
        return new RasValue(metaid, expr);
}

Ilwis::OperationImplementation::State RasValue::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    if ( _expression.parameterCount() < 3 || _expression.parameterCount() > 4) {
        ERROR3(ERR_ILLEGAL_NUM_PARM3,"rasvalue","3 or 4",QString::number(_expression.parameterCount()));
        return sPREPAREFAILED;
    }

    QString raster = _expression.parm(0).value();
    if (!_inputGC.prepare(raster)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    _pix = Pixel(_expression.parm(1).value().toInt(), _expression.parm(2).value().toInt());
    if ( _expression.parameterCount() == 4)
        _pix.z = _expression.parm(3).value().toInt();
    if (!_pix.isValid()) {
        ERROR2(ERR_INVALID_PROPERTY_FOR_2,"Pixel coordinate","rasvalue");
        return sPREPAREFAILED;
    }
    return sPREPARED;
}

quint64 RasValue::createMetadata()
{

    OperationResource operation({"ilwis://operations/rastervalue"});
    operation.setSyntax("rasvalue(inputgridcoverage,x,y,[,z])");
    operation.setDescription(TR("returns the value at pixel location x,y (optional z)"));
    operation.setInParameterCount({3|4});
    operation.addInParameter(0,itRASTER , TR("input rastercoverage"),TR("input rastercoverage with any domain"));
    operation.addInParameter(1,itINTEGER, TR("column"), TR("column number in the grid"));
    operation.addInParameter(2,itINTEGER, TR("row"), TR("row number in the grid"));
    operation.addOptionalInParameter(3,itINTEGER, TR("band number"), TR("band number in the grid"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itDOUBLE , TR("value"),TR("value at the indicated location"));
    operation.setKeywords("raster,pixel,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();

}
