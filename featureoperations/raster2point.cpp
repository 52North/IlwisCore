#include <functional>
#include <future>
#include "kernel.h"
#include "coverage.h"
#include "raster.h"
#include "pixeliterator.h"
#include "geos/geom/Point.h"
#include "geos/geom/GeometryFactory.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "table.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelperfeatures.h"
#include "raster2point.h"

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
    while( iter != iter.end()){
         double res = _inputraster->pix2value(iter.position());
         if(res != rUNDEF && res != 0){
             std::cout<<res<<std::endl;
             Pixel p = iter.position();
             Coordinate coord = _inputraster->georeference()->pixel2Coord(Pixeld(p.x,p.y));
             geos::geom::Point *point = _outputfeatures->geomfactory()->createPoint(coord);
             if(point->isValid()){
                 _outputfeatures->newFeature(point);
             }
             ++count;
         }
        ++iter;
    }
    QVariant value;
    value.setValue<IFeatureCoverage>(_outputfeatures);
    logOperation(_outputfeatures, _expression);
    ctx->setOutput(symTable,value,_outputfeatures->name(), itFEATURE, _outputfeatures->resource() );
    return true;
}

Ilwis::OperationImplementation *RasterToPoint::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RasterToPoint(metaid,expr);
}

Ilwis::OperationImplementation::State RasterToPoint::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString raster = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputraster.prepare(raster, itRASTER)) {

        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    Resource resource = outputName != sUNDEF ? Resource(INTERNAL_CATALOG + "/" + outputName, itFLATTABLE) : Resource(itFLATTABLE);
    _attTable.prepare(resource);
    IDomain covdom;
    if (!covdom.prepare("count")){
       return sPREPAREFAILED;
    }
    _inputgrf = _inputraster->georeference();
    _outputfeatures.prepare(QString(INTERNAL_CATALOG + "/%1").arg(outputName));
    _csy = _inputgrf->coordinateSystem();
    _outputfeatures->coordinateSystem(_csy);
    Envelope env = _inputraster->georeference()->envelope();
    _outputfeatures->envelope(env);
    return sPREPARED;
}

quint64 RasterToPoint::createMetadata()
{
    OperationResource operation({"ilwis://operations/raster2point"});
    operation.setSyntax("raster2point(inputraster)");
    operation.setDescription(TR("translates the pixels of a rastercoverage to points in a featurecoverage"));
    operation.setInParameterCount({1});
    operation.addInParameter(0,itRASTER, TR("input rastercoverage"),TR("input rastercoverage with any domain"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itPOINT, TR("output point coverage"), TR("output point coverage with the domain of the input map"));
    operation.setKeywords("point,raster");
    mastercatalog()->addItems({operation});
    return operation.id();
}

