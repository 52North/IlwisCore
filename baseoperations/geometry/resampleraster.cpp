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
    IRasterCoverage outputGC = _outputObj.get<RasterCoverage>();
    IRasterCoverage inputGC = _inputObj.get<RasterCoverage>();
    BoxedAsyncFunc resampleFun = [&](const Box3D<qint32>& box) -> bool {
        PixelIterator iterOut(outputGC,box);
        RasterInterpolator interpolator(inputGC, _method);
        SPRange range = inputGC->datadef().range();
        PixelIterator iterEnd = iterOut.end();
        while(iterOut != iterEnd) {
           Voxel position = iterOut.position();
           Coordinate c = outputGC->georeference()->pixel2Coord(Pixel_d(position.x(),(position.y())));
           Coordinate c2 = inputGC->coordinateSystem()->coord2coord(outputGC->coordinateSystem(),c);
           double v = interpolator.coord2value(c2);
           *iterOut = range->ensure(v);
            ++iterOut;
        }
        return true;
    };

    bool res = OperationHelperRaster::execute(ctx, resampleFun, outputGC);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(outputGC);
        ctx->addOutput(symTable,value,outputGC->name(), itRASTER, outputGC->resource() );
    }
    return res;
}

Ilwis::OperationImplementation::State ResampleRaster::prepare(ExecutionContext *, const SymbolTable & )
{
    QString rasterCoverage = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputObj.prepare(rasterCoverage, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,rasterCoverage,"");
        return sPREPAREFAILED;
    }
    _outputObj = OperationHelperRaster::initialize(_inputObj,itRASTER, itDOMAIN);
    if ( !_outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output gridcoverage");
        return sPREPAREFAILED;
    }
    IGeoReference grf;
    grf.prepare(_expression.parm(1).value());
    if ( !grf.isValid()) {
        return sPREPAREFAILED;
    }
    IRasterCoverage outputGC = _outputObj.get<RasterCoverage>();
    outputGC->georeference(grf);
    Box2Dd env = grf->pixel2Coord(grf->size());
    outputGC->envelope(env);
    if ( outputName != sUNDEF)
        outputGC->setName(outputName);

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
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","resample");
    res.addProperty("syntax","resample(inputgridcoverage,targetgeoref,nearestneighbour|bilinear|bicubic)");
    res.addProperty("description",TR("translates a gridcoverage from one geometry (coordinatesystem+georeference) to another"));
    res.addProperty("inparameters","3");
    res.addProperty("pin_1_type", itRASTER);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with domain any domain"));
    res.addProperty("pin_2_type", itGEOREF);
    res.addProperty("pin_2_name", TR("target georeference"));
    res.addProperty("pin_2_desc",TR("the georeference to which the input coverage will be morphed"));
    res.addProperty("pin_3_type", itSTRING);
    res.addProperty("pin_3_name", TR("Resampling method"));
    res.addProperty("pin_3_desc",TR("The method used to aggregate pixels from the input map in the geometry of the output map"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itRASTER);
    res.addProperty("pout_1_name", TR("output gridcoverage"));
    res.addProperty("pout_1_desc",TR("output gridcoverage with the domain of the input map"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}


