#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "featurecoverage.h"
#include "pixeliterator.h"
#include "featureiterator.h"
#include "feature.h"
#include "geos/geom/GeometryFactory.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "table.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operationhelperfeatures.h"
#include "rastertopoint.h"

using namespace Ilwis;
using namespace FeatureOperations;

REGISTER_OPERATION(RasterToPoint)

RasterToPoint::RasterToPoint()
{

}

RasterToPoint::~RasterToPoint()
{

}

RasterToPoint::RasterToPoint(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr)
{

}


bool RasterToPoint::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    PixelIterator iter(_inputraster);
    int count = 0;
    SPFeatureI newFeature;
    while( iter != _inputraster->end()){
         Pixel p = iter.position();;
         Coordinate coord = _inputraster->georeference()->pixel2Coord(Pixeld(p.x,p.y));
         QVariant v= _inputraster->coord2value(coord);
         newFeature(count,v);
         if(*iter >= 0){
              geos::geom::Point *pol = _outputfeatures->geomfactory()->createPoint(coord);
         }
        ++iter;
         count++;
    }
    if ( ctx != 0) {
        QVariant value;
        value.setValue<IFeatureCoverage>(_outputfeatures);
        ctx->setOutput(symTable,value,_outputfeatures->name(), itFEATURE, _outputfeatures->source() );
    }
    return true;
}

Ilwis::OperationImplementation *RasterToPoint::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RasterToPoint(metaid,expr);
}

Ilwis::OperationImplementation::State RasterToPoint::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString raster = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();
    if (!_inputraster.prepare(raster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    _outputfeatures = OperationHelperFeatures::initialize(_inputraster,itPOINT,itCOORDSYSTEM | itDOMAIN | itENVELOPE);
    if (outputName != sUNDEF){
        _outputfeatures->name(outputName);
        _outputfeatures->attributeTable()->name(outputName);
    }
    _doCoordTransform = _inputraster->coordinateSystem() != _outputfeatures->coordinateSystem();

    return sPREPARED;
}

quint64 RasterToPoint::createMetadata()
{
    OperationResource operation({"ilwis://operations/raster2point"});
    operation.setLongName("Raster to point map");
    operation.setKeywords("point,raster");
    operation.setSyntax("raster2point(input-raster)");
    operation.setDescription(TR("translates the pixels of a rastercoverage to points in a featurecoverage"));
    operation.setInParameterCount({1});
    operation.addInParameter(0,itRASTER, TR("input rastercoverage"),TR("input rastercoverage with any domain"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itPOINT, TR("output featurecoverage"), TR("output feauturecoverage with the domain of the input map"));
    mastercatalog()->addItems({operation});

    return operation.id();
}
