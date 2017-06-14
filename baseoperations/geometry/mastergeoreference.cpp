#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "mastergeoreference.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(MasterGeoreference)

MasterGeoreference::MasterGeoreference()
{
}

Ilwis::OperationImplementation *MasterGeoreference::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MasterGeoreference(metaid, expr);
}

MasterGeoreference::MasterGeoreference(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool MasterGeoreference::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    ctx->_masterGeoref = _georef->resource().url().toString();

    return true;
}

Ilwis::OperationImplementation::State MasterGeoreference::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
   QString georef = _expression.parm(0).value();
   if(!_georef.prepare(georef)) {
       ERROR2(ERR_COULD_NOT_LOAD_2,georef,"");
       return sPREPAREFAILED;
   }
   return sPREPARED;
}

quint64 MasterGeoreference::createMetadata()
{
    OperationResource operation({"ilwis://operations/mastergeoreference"});
    operation.setSyntax("mastergeoref(targetgeoref)");
    operation.setDescription(TR("Sets the georeference for all operations that trigger an on the fly resampling of a gridcoverage"));
    operation.setInParameterCount({1});
    operation.addInParameter(0,itRASTER|itGEOREF, TR("georeference"),TR("input georeference or raster, in which case it takes the georeference of the raster"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itUNKNOWN, TR(""));
    operation.setKeywords("georeference,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();
}
