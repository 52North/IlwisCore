#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "mastergeoreference.h"

using namespace Ilwis;
using namespace BaseOperations;

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

    ctx->_masterGeoref = _georef->source().url().toString();

    return true;
}

Ilwis::OperationImplementation::State MasterGeoreference::prepare(ExecutionContext *ctx, const SymbolTable &)
{
   QString georef = _expression.parm(0).value();
   if(!_georef.prepare(georef)) {
       ERROR2(ERR_COULD_NOT_LOAD_2,georef,"");
       return sPREPAREFAILED;
   }
   return sPREPARED;
}

quint64 MasterGeoreference::createMetadata()
{
    QString url = QString("ilwis://operations/mastergeoreference");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","mastergeoreference");
    resource.addProperty("syntax","mastergeoref(rastercoverage|targetgeoref)");
    resource.addProperty("description",TR("Sets the georeference for all operations that trigger an on the fly resampling of a gridcoverage"));
    resource.addProperty("inparameters","1");
    resource.addProperty("pin_1_type", itGEOREF | itRASTER);
    resource.addProperty("pin_1_name", TR("georeference"));
    resource.addProperty("pin_1_desc",TR("input georeference or raster, in which case it takes the georeference of the raster"));
    resource.addProperty("outparameters",0);
    resource.addProperty("pout_1_type", itUNKNOWN);
    resource.addProperty("pout_1_name", TR(""));
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}
