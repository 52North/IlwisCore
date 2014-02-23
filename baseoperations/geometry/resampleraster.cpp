#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "rasterinterpolator.h"
#include "resampleraster.h"

using namespace Ilwis;
using namespace BaseOperations;


Ilwis::OperationImplementation *ResampleRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ResampleRaster(metaid, expr);
}

ResampleRaster::ResampleRaster()
{
}

ResampleRaster::ResampleRaster(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr),
    _method(RasterInterpolator::ipBICUBIC)
{
}

bool ResampleRaster::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    IRasterCoverage outputRaster = _outputObj.get<RasterCoverage>();
    IRasterCoverage inputRaster = _inputObj.get<RasterCoverage>();
    SPTranquilizer trq = kernel()->createTrq("resample", "", outputRaster->size().ysize(),1);

    BoxedAsyncFunc resampleFun = [&](const BoundingBox& box) -> bool {
        PixelIterator iterOut(outputRaster,box);
        iterOut.setTranquilizer(trq);
        RasterInterpolator interpolator(inputRaster, _method);
        SPRange range = inputRaster->datadef().range();
        PixelIterator iterEnd = iterOut.end();
        bool equalCsy = inputRaster->coordinateSystem()->isEqual(outputRaster->coordinateSystem().ptr());
        while(iterOut != iterEnd) {
            Pixel position = iterOut.position();
            Coordinate coord = outputRaster->georeference()->pixel2Coord(Pixeld(position.x,(position.y)));
            if ( !equalCsy)
                coord = inputRaster->coordinateSystem()->coord2coord(outputRaster->coordinateSystem(),coord);
            *iterOut = interpolator.coord2value(coord);
            ++iterOut;
        }
        return true;
    };

    bool resource = OperationHelperRaster::execute(ctx, resampleFun, outputRaster);

    if ( resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(outputRaster);
        ctx->setOutput(symTable,value,outputRaster->name(), itRASTER, outputRaster->source() );
    }
    return resource;
}

Ilwis::OperationImplementation::State ResampleRaster::prepare(ExecutionContext *, const SymbolTable & )
{
    QString raster = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputObj.prepare(raster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
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
    Envelope env = grf->pixel2Coord(grf->size());
    outputRaster->envelope(env);
    if ( outputName != sUNDEF)
        outputRaster->setName(outputName);

    QString method = _expression.parm(2).value();
    if ( method.toLower() == "nearestneighbour")
        _method = RasterInterpolator::ipNEARESTNEIGHBOUR;
    else if ( method.toLower() == "bilinear")
        _method = RasterInterpolator::ipBILINEAR;
    else if (  method.toLower() == "bicubic")
        _method =RasterInterpolator::ipBICUBIC;
    else {
        ERROR3(ERR_ILLEGAL_PARM_3,"method",method,"resample");
        return sPREPAREFAILED;
    }

    return sPREPARED;
}

quint64 ResampleRaster::createMetadata()
{
    QString url = QString("ilwis://operations/resample");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","resample");
    resource.addProperty("syntax","resample(inputgridcoverage,targetgeoref,nearestneighbour|bilinear|bicubic)");
    resource.addProperty("description",TR("translates a rastercoverage from one geometry (coordinatesystem+georeference) to another"));
    resource.addProperty("inparameters","3");
    resource.addProperty("pin_1_type", itRASTER);
    resource.addProperty("pin_1_name", TR("input rastercoverage"));
    resource.addProperty("pin_1_desc",TR("input rastercoverage with domain any domain"));
    resource.addProperty("pin_2_type", itGEOREF);
    resource.addProperty("pin_2_name", TR("target georeference"));
    resource.addProperty("pin_2_desc",TR("the georeference to which the input coverage will be morphed"));
    resource.addProperty("pin_3_type", itSTRING);
    resource.addProperty("pin_3_name", TR("Resampling method"));
    resource.addProperty("pin_3_desc",TR("The method used to aggregate pixels from the input map in the geometry of the output map"));
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itRASTER);
    resource.addProperty("pout_1_name", TR("output rastercoverage"));
    resource.addProperty("pout_1_desc",TR("output rastercoverage with the domain of the input map"));
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}


