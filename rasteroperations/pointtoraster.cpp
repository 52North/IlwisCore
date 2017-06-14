#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "featurecoverage.h"
#include "table.h"
#include "feature.h"
#include "featureiterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pointtoraster.h"
using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(PointToRaster)

PointToRaster::PointToRaster()
{

}

PointToRaster::~PointToRaster()
{

}

PointToRaster::PointToRaster(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr)
{

}


bool PointToRaster::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    // we create a pixeliterator to move around on the output raster coverage
    PixelIterator pixiter(_outputraster);
    // create an empty raster (all pixels nodata)
    PixelIterator end = _outputraster->end();
    while(pixiter != end) {
        *pixiter = rUNDEF;
        ++pixiter;
    }
    // we need a coordinate transformation when the two coordinatesystem dont match
    bool needCoordinateTransformation = _inputgrf->coordinateSystem() != _inputfeatures->coordinateSystem();
    // loop over all features
    // the 'feature' object is actually something of the SPFeatureI class which is a simple wrapper for a featureinterface
    for(auto feature :  _inputfeatures){
        // we are only interested in points, other geometry types are skipped.
        if ( feature->geometryType() != itPOINT)
            continue;
        // get the coordinate of the feature. fortunately points are simple, only one coordinate. Note that we might have multipoints but for
        // the simplicity of the example we ignore that for the moment
        Coordinate crd(*feature->geometry()->getCoordinate());
        // if the coordinate system of input and output dont match we need to transform the input coordinate to something that
        // makes sense in the system of the output raster
        if ( needCoordinateTransformation)
            crd = _outputraster->coordinateSystem()->coord2coord(_inputfeatures->coordinateSystem(), crd);
        // a raster contains pixels, each pixel represents a location(world coordinate) in the coordinate system of the raster
        // the georefence translate (both directions) between pixel locations and world locations
        // now we move the pixel iterator to the right location. normally we use operators like ++, --. +=, -= for this job
        // but as a pointmap is representing a very sparsely filled raster we can use a slightly less performant operator (assignment)
        // to keep the code simple. There are not that many pixels to be filled
        if (crd.isValid()) {
            Pixel pix = _outputraster->georeference()->coord2Pixel(crd);
            if (pix.isValid() && pixiter.contains(pix)) {
                pixiter = pix;
                // and we assign a value to the pixel
                quint64 id = feature->featureid();
                *pixiter = id;
            }
        }
    }
    // we are done now. We need to transfer the generated output to the system so that other parts can also use it.
    // This is mainly relevant when operations are used in a scripting enviroment as the output has to be known in the context
    // were the script is running.
    QVariant value;
    value.setValue<IRasterCoverage>( _outputraster  );
    logOperation(_outputraster,_expression);
    ctx->setOutput(symTable,value,_outputraster->name(), itRASTER, _outputraster->resource() );
    return true;
}

Ilwis::OperationImplementation *PointToRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new PointToRaster(metaid,expr);
}

Ilwis::OperationImplementation::State PointToRaster::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    // according to syntax specification the first input parameter will be  the name of the featurecoverage
    // used in this operation. Usually this will be a full url ( eg.g file://d:/somedata/mycoverage.shp
    // remember Ilwis-objects uses urls for locations, not file paths
    QString features = _expression.parm(0).value();
    // The first (and only) output parameter is the name of the output object. There is no need to do this as
    // an url because output objects only exist in memory. You can save them to a file or put them in a database
    // but that is not specified at this step. The name is just used for reference and recognition
    QString outputName = _expression.parm(0,false).value();

    // if the input feature coverage is not valid, we abort. No operation possible
    if (!_inputfeatures.prepare(features, itFEATURE)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,features,"");
        return sPREPAREFAILED;
    }

    // in the first variant of the operation we only have two parameters, the georef case
    // note that the georef might have a different coordinate system as the input featurecoverage
    // this means that the coordinates in the input are in a different system as the coordinate in the
    // output. This means that we have to translate input coordinate to output coordinates. we will do this later
    if ( _expression.parameterCount() == 2){ // the georef case
        QString georefname = _expression.parm(1).value();
        if (!_inputgrf.prepare(georefname, itGEOREF)) {
            ERROR2(ERR_COULD_NOT_LOAD_2,georefname,"");
            return sPREPAREFAILED;
        }

    }else if ( _expression.parameterCount() == 3){
        // in the second case we have to create a standalone georeference. On the one hand this is a little bit more
        // complicated, on the other hand we are certain that the coordinate in input and output are in the same system
        // so no transformations needed. The georeference will always be of the "corners" type
        bool ok;
        // retrieve the x and y size of the to be created georeference
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
        // initialize the georeference, with name, bounding box, coordinatesystem and envelope
        IGeoReference grf(outputName);
        grf->create("corners");
        grf->size(Size<>(xsize, ysize,1)); // sets the bounding box
        grf->coordinateSystem(_inputfeatures->coordinateSystem());
        grf->envelope(_inputfeatures->envelope());
        grf->compute(); // all members are set, now the initialization can take place
        _inputgrf = grf;
    }

    // we handle the domain a little but simplistic. In reality the domain has to be extracted from the featurecoverage
    // but that complicates things for this example. We choose a value domain to support rUNDEF.
    IDomain dom("value");
    // initialize the output rastercoverage.
    _outputraster.prepare();
    if (outputName != sUNDEF)
         _outputraster->name(outputName);

    _outputraster->coordinateSystem(_inputgrf->coordinateSystem());
    // we need to set the envelope of the output raster. For this we need to convert the envelope of the input to the output. If the
    // coordinatesystem of both coverage are the same this is ofcourse a simple copy, else a real coordinate transformation will be done
    Envelope env = _inputgrf->coordinateSystem()->convertEnvelope(_inputfeatures->coordinateSystem(), _inputfeatures->envelope());
    _outputraster->envelope(env);
    _outputraster->georeference(_inputgrf);
    std::vector<double> indexes = {0};
    _outputraster->setDataDefintions(dom,indexes);
    return sPREPARED;
}

quint64 PointToRaster::createMetadata()
{
    OperationResource operation({"ilwis://operations/point2raster"});
    operation.setLongName("Point to raster map");
    operation.setKeywords("raster,point");
    /*
     * the expression can use a pointmap as input and either a georeference or a x/y extent. In the first case the georeference has to be defined
     * outside the application. The coordinates of the input points will be transformed to the coordinate system of the input georeference (which might
     * be different). The extent of the new raster will be the size of the georeference. If the second options is choosen the output raster will have the same
     * coordinate system as the input featurecoverage and an x and y size (in pixels) as defined by the parameters. In effect a new Corners georeference
     * will be created on the fly
    */
    operation.setSyntax("point2raster(inputpointmap,targetgeoref | xsize[,ysize])");
    operation.setDescription(TR("translates a the points of a featurecoverage to pixels in a rastermap"));
    /*
     * so the expression has either 2 input parameters. A featurecoverage and georeference. Or 3 parameters, a featurecoverage, x extent (in pixels) and y extent (in pixels)
    */
    operation.setInParameterCount({2,3});
    /*
     * for the input type itPOINT is specified. All coverages of the type itPOINT are featurecoverage and in effect the (aggregated) type itFEATURE is the
     * logical combination itFEATURE = itPOINT | itLINE | itPOLYGON
    */
    operation.addInParameter(0,itPOINT, TR("input featurecoverage"),TR("input featurecoverage with any domain"));
    /*
     * There are two possible types of parameters here. It is either a georeference in the case of the 2 parameter variant or an integer for the x size
     * in the case of the three parameter variant
     */
    operation.addInParameter(1,itGEOREF | itINTEGER, TR("input georeference or x size"),TR("the parameter can either be a georeference or the x extent of the the to be created raster"));
    /*
     * third parameter will only be there when the size of the input map will be given as an x/y pair
     */
    operation.addInParameter(2,itINTEGER, TR("input y size"),TR("optional y size of the output raster. Only used of the previous parameter was an x size"));
    /*
     * The operation will have only one output parameter a raster
     */
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"), TR("output rastercoverage with the domain of the input map"));
    mastercatalog()->addItems({operation});

    return operation.id();
}

