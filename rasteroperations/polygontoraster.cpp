#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "rastercoverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "table.h"
#include "pixeliterator.h"
#include "featureiterator.h"
#include "symboltable.h"
#include "attributetable.h"
#include "ilwisoperation.h"
#include "geos/geom/Envelope.h"
#include "bresenham.h"
#include "vertexiterator.h"
#include "polygontoraster.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(PolygonToRaster)


PolygonToRaster::PolygonToRaster()
{

}

PolygonToRaster::~PolygonToRaster()
{

}

PolygonToRaster::PolygonToRaster(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr)
{

}

bool PolygonToRaster::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    Ilwis::Bresenham algo(_outputraster->georeference());
    PixelIterator pixiter(_outputraster);
    initialize(_inputfeatures->featureCount(itPOLYGON));
    quint64 count = 0;
    for(auto feature :  _inputfeatures){
        if ( feature->geometryType() != itPOLYGON){

           continue;
        }        
        VertexIterator vertices(feature->geometry());
        std::vector<Pixel> selectionPix = algo.rasterize(::begin(vertices),::end(vertices));
        Pixel pix;
        for(int i = 0; i< selectionPix.size(); i++){
            pix = selectionPix[i];
            // the pix may contain a fake z coordinate as this represents the index of the polygon in a set of polygons of a multi polygon
            // we copy the z value of the output raster here
            pixiter = Pixel(pix.x, pix.y, pixiter.z());
            *pixiter = -1;
        }
        updateTranquilizer(++count,10);
    }
    Pixel pix;
    long xsize =_inputgrf->size().xsize();
    long ysize =_inputgrf->size().ysize();
    initialize(ysize);
    double mmax = -1e308, mmin = 1e308;
    QString valueColumn = _inputfeatures->attributeDefinitions().columnIndex(FEATUREVALUECOLUMN) != iUNDEF ? FEATUREVALUECOLUMN : COVERAGEKEYCOLUMN;

    for (long y = 0; y < ysize; ++y) {
         pix.y = y;
            // detect the polygon-borders (pixels with value -1), line-by-line
        std::vector<long> borders;
        borders.push_back(0); // add the left side of the!
        for (long x = 0; x < xsize; ++x) {
             pix.x = x; // y is already set
             pixiter = pix;
             if (*pixiter == -1)
                 borders.push_back(x); // each polygon boundary
        }
        borders.push_back(xsize); //

        for (int i = 1; i < borders.size(); ++i) {
             long middle = (borders[i-1] + borders[i] )/ 2;
             double value;
             Pixel position (middle + 0.5, y + 0.5); // take the polygon-value from the middle between two borders
             Coordinate crd = _inputgrf->pixel2Coord(position);

             QVariant d = _inputfeatures->coord2value(crd);
             if (d.isValid()){
                QVariantMap vmap = d.value<QVariantMap>();
                QVariant attribute =  vmap[valueColumn];
                value = attribute.toInt();
             }else{
                value = rUNDEF;
             }
             mmax = Ilwis::max(mmax, value);
             mmin = Ilwis::min(mmin, value);
             for(long x=borders[i-1]; x < borders[i]; ++x) {
                 pix.x = x; // y is already set
                 pixiter = pix;
                 *pixiter = value;
             }
        }
        updateTranquilizer(y,1);
    }
    _outputraster->setAttributes(_inputfeatures->attributeTable().as<AttributeTable>()->copyTable(_outputraster->name()));

    QVariant value;
    value.setValue<IRasterCoverage>(_outputraster);
    logOperation(_outputraster,_expression);
    ctx->setOutput(symTable,value,_outputraster->name(), itRASTER, _outputraster->resource() );

    return true;
}

Ilwis::OperationImplementation *PolygonToRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new PolygonToRaster(metaid, expr);
}

Ilwis::OperationImplementation::State PolygonToRaster::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString features = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputfeatures.prepare(features, itPOLYGON)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,features,"");
        return sPREPAREFAILED;
    }

    if ( _expression.parameterCount() == 2){
        QString georefname = _expression.parm(1).value();
        if (!_inputgrf.prepare(georefname, itGEOREF)) {
            ERROR2(ERR_COULD_NOT_LOAD_2,georefname,"");
            return sPREPAREFAILED;
        }
    }
    _needCoordinateTransformation = _inputgrf->coordinateSystem() != _inputfeatures->coordinateSystem();
    _getsAttributeTable =  _inputfeatures->attributeDefinitions().columnCount() > 1;
    IDomain dom;
     dom = _inputfeatures->attributeDefinitions().columndefinition(FEATUREVALUECOLUMN).datadef().domain();
     if ( !dom.isValid()){
         dom = _inputfeatures->attributeDefinitions().columndefinition(COVERAGEKEYCOLUMN).datadef().domain();
     }

    _outputraster.prepare();
    if (outputName != sUNDEF)
         _outputraster->name(outputName);

    _outputraster->coordinateSystem(_inputgrf->coordinateSystem());
    Envelope env = _inputgrf->coordinateSystem()->convertEnvelope(_inputfeatures->coordinateSystem(), _inputfeatures->envelope());
    _outputraster->envelope(env);
    _outputraster->georeference(_inputgrf);
    std::vector<double> indexes = {0};
    _outputraster->setDataDefintions(dom,indexes);
    return sPREPARED;
}

quint64 PolygonToRaster::createMetadata()
{
    OperationResource operation({"ilwis://operations/polygon2raster"});
    operation.setLongName("Polygon to raster map");
    operation.setSyntax("polygon2raster(input-polygonmap,targetgeoref)");
    operation.setDescription(TR("translates a the points of a featurecoverage to pixels in a rastermap"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itPOLYGON, TR("input featurecoverage"),TR("input featurecoverage with any domain"));
    operation.addInParameter(1,itGEOREF, TR("input georeference"),TR("Determines the geometry of the output raster"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"), TR("output rastercoverage with the domain of the input map"));
    operation.setKeywords("raster,polygoncoverage");
    mastercatalog()->addItems({operation});
    return operation.id();
}
