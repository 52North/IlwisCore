
#include "kernel.h"
#include "raster.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "rasterstrechoperation.h"

using namespace Ilwis;
using namespace RasterOperations;

LinearStretchOperation::LinearStretchOperation()
{
}

LinearStretchOperation::LinearStretchOperation(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

Ilwis::OperationImplementation *LinearStretchOperation::create(quint64 metaid,const Ilwis::OperationExpression& expr)
{
    return new LinearStretchOperation(metaid, expr);
}

bool LinearStretchOperation::stretch(IRasterCoverage toStretch)
{

    // TODO do _statistics histogram calculation

    return false;
}

bool LinearStretchOperation::execute(ExecutionContext *ctx,SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    // TODO do linear stretch of histogram

    return false;
}

Ilwis::OperationImplementation::State LinearStretchOperation::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString raster = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputObj.prepare(raster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }

    // TODO prepare output raster
    _outputObj = OperationHelperRaster::initialize(_inputObj,itRASTER, itDOMAIN);
    if ( !_outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }
    IGeoReference grf;
    grf.prepare(_expression.parm(1).value());
    if ( !grf.isValid()) {
        return sPREPAREFAILED;
    }
    IRasterCoverage outputRaster = _outputObj.get<RasterCoverage>();
    outputRaster->georeference(grf);
    Box2Dd env = grf->pixel2Coord(grf->size());
    outputRaster->envelope(env);
    if ( outputName != sUNDEF)
        outputRaster->setName(outputName);

    /*
    RasterCoverage output;
    QString outputId = QString("ilwis://internalcatalog/%1").arg(outputName);
    if (!output.prepare(outputId, itRASTER)) {
        ERROR1(ERR_NO_INITIALIZED_1,outputName);
        return sPREPAREFAILED;
    }
    */


    return sNOTPREPARED;
}

quint64 LinearStretchOperation::createMetadata()
{
    QString url = QString("ilwis://operations/stretch");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","rescale input values to an output map");
    resource.addProperty("syntax","stretch(raster, range");
    resource.addProperty("description",TR("re-distributes values of an input map over a wider or narrower range of values in an output map. Stretching can for instance be used to enhance the contrast in your map when it is displayed."));
    resource.addProperty("inparameters","1");
    resource.addProperty("pin_1_type", itRASTER);
    resource.addProperty("pin_1_name", TR("rastercoverage to stretch"));
    resource.addProperty("pin_1_desc",TR("input rastercoverage with domain item or numeric"));
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itRASTER);
    resource.addProperty("pout_1_name", TR("output rastercoverage"));
    resource.addProperty("pout_1_desc",TR("output rastercoverage stretched"));
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}


