#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "gridinterpolator.h"
#include "aggregateraster.h"

using namespace Ilwis;
using namespace RasterOperations;


Ilwis::OperationImplementation *AggregateRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new AggregateRaster(metaid, expr);
}

AggregateRaster::AggregateRaster()
{
}

AggregateRaster::AggregateRaster(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr),
    _method(GridInterpolator::ipBICUBIC)
{
}

bool AggregateRaster::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    IGridCoverage outputGC = _outputObj.get<GridCoverage>();
    IGridCoverage inputGC = _inputObj.get<GridCoverage>();
    BoxedAsyncFunc resampleFun = [&](const Box3D<qint32>& box) -> bool {
        PixelIterator iterOut(outputGC,box);
        GridInterpolator interpolator(inputGC, _method);
        SPRange range = inputGC->datadef().range();
        while(iterOut != iterOut.end()) {
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
        value.setValue<IGridCoverage>(outputGC);
        ctx->addOutput(symTable,value,outputGC->name(), itGRID, outputGC->resource() );
    }
    return res;
}

Ilwis::OperationImplementation::State AggregateRaster::prepare(ExecutionContext *, const SymbolTable & )
{
    QString gc = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputObj.prepare(gc, itGRID)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
        return sPREPAREFAILED;
    }

    _outputObj = OperationHelperRaster::initialize(_inputObj,itGRID, itDOMAIN | itCOORDSYSTEM);
    if ( !_outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output gridcoverage");
        return sPREPAREFAILED;
    }
    IGeoReference grf;
    grf.prepare(_expression.parm(1).value());
    if ( !grf.isValid()) {
        return sPREPAREFAILED;
    }
    IGridCoverage outputGC = _outputObj.get<GridCoverage>();
    outputGC->georeference(grf);
    Box2Dd env = grf->pixel2Coord(grf->size());
    outputGC->envelope(env);
    if ( outputName != sUNDEF)
        outputGC->setName(outputName);

    QString method = _expression.parm(2).value();
    if ( method.toLower() == "nearestneighbour")
        _method = GridInterpolator::ipNEARESTNEIGHBOUR;
    else if ( method.toLower() == "bilinear")
        _method = GridInterpolator::ipBILINEAR;
    else if (  method.toLower() == "bicubic")
        _method =GridInterpolator::ipBICUBIC;
    else {
        ERROR3(ERR_ILLEGAL_PARM_3,"method",method,"resample");
        return sPREPAREFAILED;
    }

    return sPREPARED;
}

quint64 AggregateRaster::createMetadata()
{
    QString url = QString("ilwis://operations/aggregateraster");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","aggregateraster raster coverage");
    res.addProperty("syntax","resample(inputgridcoverage,{Avg|Max|Med|Min|Prd|Std|Sum}, groupsize,changegeometry[,new georefname])");
    res.addProperty("inparameters","3");
    res.addProperty("pin_1_type", itGRID);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with domain any domain"));
    res.addProperty("pin_2_type", itSTRING);
    res.addProperty("pin_2_name", TR("Aggregation Method"));
    res.addProperty("pin_2_desc",TR("the method how pixels inside a group will be accumulated"));
    res.addProperty("pin_3_type", itINTEGER);
    res.addProperty("pin_3_name", TR("Groupsize"));
    res.addProperty("pin_3_desc",TR("The size of the block used to aggregate"));
    res.addProperty("pin_4_type", itBOOL);
    res.addProperty("pin_4_name", TR("change geometry"));
    res.addProperty("pin_4_desc",TR("The aggregation can either create a map with a reduced size proportional to de block size or use the same geometry size but fill all pixels in the block with the aggregate"));
    res.addProperty("pin_5_type", itSTRING);
    res.addProperty("pin_5_name", TR("georeference name"));
    res.addProperty("pin_5_desc",TR("optional parameter indicating a name for the new geometry, else the name will come from the output grid"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itGRID);
    res.addProperty("pout_1_name", TR("output gridcoverage"));
    res.addProperty("pout_1_desc",TR("output gridcoverage with the domain of the input map"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}


