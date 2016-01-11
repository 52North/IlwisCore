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
#include "ilwisoperation.h"
#include "geos/geos/geom/Envelope.inl"
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
    for(auto feature :  _inputfeatures){
        if ( feature->geometryType() != itPOLYGON){

           continue;
        }        
        VertexIterator vertices(feature->geometry());
        std::vector<Pixel> selectionPix = algo.rasterize(::begin(vertices),::end(vertices));
        Pixel pix;
        for(int i = 0; i< selectionPix.size(); i++){
            pix = selectionPix[i];
            pixiter = pix;
            *pixiter = -1;
        }
    }
    Pixel pix;
    long xsize =_inputgrf->size().xsize();
    long ysize =_inputgrf->size().ysize();
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
             int value;
             Pixel position (middle + 0.5, y + 0.5); // take the polygon-value from the middle between two borders
             Coordinate crd = _inputgrf->pixel2Coord(position);

             QVariant d = _inputfeatures->coord2value(crd);
             if (d.isValid()){
                QVariantMap vmap = d.value<QVariantMap>();
                QVariant attribute =  vmap[COVERAGEKEYCOLUMN];
                value = attribute.toInt()+1;
             }else{
                value = 0;
             }
             for(long x=borders[i-1]; x < borders[i]; ++x) {
                 pix.x = x; // y is already set
                 pixiter = pix;
                 *pixiter = value;
             }
        }
    }

    QVariant value;
    value.setValue<IFeatureCoverage>(_inputfeatures);
    ctx->setOutput(symTable,value,_outputraster->name(), itRASTER, _outputraster->source() );

    return true;
}

Ilwis::OperationImplementation *PolygonToRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new PolygonToRaster(metaid, expr);
}

Ilwis::OperationImplementation::State PolygonToRaster::prepare(ExecutionContext *ctx, const SymbolTable &)
{
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

    }else if ( _expression.parameterCount() == 3){
        bool ok;
        int xsize = _expression.parm(1).value().toULong(&ok);
        if ( !ok){
           ERROR2(ERR_ILLEGAL_VALUE_2,TR("parameter"),_expression.parm(1).value());
           return sPREPAREFAILED;
        }
        int ysize = _expression.parm(2).value().toULong(&ok);
        if ( !ok){
           ERROR2(ERR_ILLEGAL_VALUE_2,TR("parameter"),_expression.parm(2).value());
           return sPREPAREFAILED;
        }

        IGeoReference grf(outputName);
        grf->create("corners");
        grf->size(Size<>(xsize, ysize,1));
        grf->coordinateSystem(_inputfeatures->coordinateSystem());
        grf->envelope(_inputfeatures->envelope());
        grf->compute();
        _inputgrf = grf;
    }
     _needCoordinateTransformation = _inputgrf->coordinateSystem() != _inputfeatures->coordinateSystem();
    IDomain dom("code=count");
    _outputraster = IRasterCoverage(outputName);
    _outputraster->datadefRef().domain(dom);   
    _outputraster->coordinateSystem(_inputgrf->coordinateSystem());
    env = _inputgrf->coordinateSystem()->convertEnvelope(_inputfeatures->coordinateSystem(), _inputfeatures->envelope());
    _outputraster->envelope(env);
    _outputraster->georeference(_inputgrf);
    return sPREPARED;
}

quint64 PolygonToRaster::createMetadata()
{
    OperationResource operation({"ilwis://operations/polygon2raster"});
    operation.setLongName("Polygon to raster map");
    operation.setSyntax("polygon2raster(input-polygonmap,targetgeoref | xsize[,ysize])");
    operation.setDescription(TR("translates a the points of a featurecoverage to pixels in a rastermap"));
    operation.setInParameterCount({2,3});
    operation.addInParameter(0,itPOLYGON, TR("input featurecoverage"),TR("input featurecoverage with any domain"));
    operation.addInParameter(1,itGEOREF | itINTEGER, TR("input georeference or x size"),TR("the parameter can either be a georeference or the x extent of the the to be created raster"));
    operation.addInParameter(2,itINTEGER, TR("input y size"),TR("optional y size of the output raster. Only used of the previous parameter was an x size"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"), TR("output rastercoverage with the domain of the input map"));
    operation.setKeywords("raster,polygonmap");
    mastercatalog()->addItems({operation});
    return operation.id();
}
