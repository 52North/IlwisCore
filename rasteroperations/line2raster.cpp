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
#include "vertexiterator.h"
#include "bresenham.h"
#include "line2raster.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(Line2Raster)

Line2Raster::Line2Raster()
{

}

Line2Raster::~Line2Raster()
{

}

Line2Raster::Line2Raster(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr)
{

}


bool Line2Raster::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;


    // loop over all features
    // the 'feature' object is actually something of the SPFeatureI class which is a simple wrapper for a featureinterface

    PixelIterator pixOut(_outputraster);
    Bresenham br(_outputraster->georeference(), _inputfeatures->coordinateSystem());
    double mmin=1e308, mmax=-1e308;
    for(auto feature :  _inputfeatures){
        if ( feature->geometryType() != itLINE)
            continue;

        VertexIterator vertStart(feature->geometry());
        VertexIterator vertEnd = ::end(vertStart);
        std::vector<Pixel> result = br.rasterize(vertStart, vertEnd);
        for(const Pixel& pix : result){
            pixOut = pix;
            double v = feature->cell(_primaryColumnIndex).toDouble();
            mmin = Ilwis::min(mmin, v);
            mmax = Ilwis::max(mmax, v);
            *pixOut = v;

        }
    }
    if ( hasType(_outputraster->datadef().domain()->ilwisType(), itNUMERICDOMAIN)){
        double prec = _datadef.range()->as<NumericRange>()->resolution();
        NumericRange *rng = new NumericRange(mmin, mmax, prec);
        _outputraster->datadefRef().range(rng);
    }

    QVariant value;
    value.setValue<IRasterCoverage>( _outputraster  );
    logOperation(_outputraster,_expression);
    ctx->setOutput(symTable,value,_outputraster->name(), itRASTER, _outputraster->resource() );
    return true;
}

Ilwis::OperationImplementation *Line2Raster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Line2Raster(metaid,expr);
}

Ilwis::OperationImplementation::State Line2Raster::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString features = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    // if the input feature coverage is not valid, we abort. No operation possible
    if (!_inputfeatures.prepare(features, itFEATURE)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,features,"");
        return sPREPAREFAILED;
    }
    QString georefname = _expression.parm(1).value();
    if (!_inputgrf.prepare(georefname, itGEOREF)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,georefname,"");
        return sPREPAREFAILED;
    }
    if ( (_primaryColumnIndex = _inputfeatures->attributeDefinitions().columnIndex(FEATUREVALUECOLUMN)) != iUNDEF){
        _datadef = _inputfeatures->attributeDefinitions().columndefinition(FEATUREVALUECOLUMN).datadef();
    }else if ( (_primaryColumnIndex =_inputfeatures->attributeDefinitions().columnIndex(COVERAGEKEYCOLUMN)) != iUNDEF){
        _datadef = _inputfeatures->attributeDefinitions().columndefinition(COVERAGEKEYCOLUMN).datadef();
    }else {
        kernel()->issues()->log(TR("No suitable primary key column found"));
        return sPREPAREFAILED;
    }
   // initialize the output rastercoverage.
     _outputraster.prepare();
    if (outputName != sUNDEF)
         _outputraster->name(outputName);

    _outputraster->coordinateSystem(_inputgrf->coordinateSystem());
    auto env = _inputgrf->coordinateSystem()->convertEnvelope(_inputfeatures->coordinateSystem(), _inputfeatures->envelope());
    _outputraster->envelope(env);
    _outputraster->georeference(_inputgrf);
    std::vector<double> indexes = {0};
    _outputraster->setDataDefintions(_datadef.domain(),indexes);
    if ( _inputfeatures->attributeDefinitions().columnIndex(COVERAGEKEYCOLUMN) != iUNDEF){

        _outputraster->setAttributes(_inputfeatures->attributeTable()->copyTable(outputName != sUNDEF ? outputName : sUNDEF));
    }

    return sPREPARED;
}

quint64 Line2Raster::createMetadata()
{
    OperationResource operation({"ilwis://operations/line2raster"});
    operation.setLongName("Rasterize Lines");
    operation.setKeywords("raster,line");
    /*
     * the expression can use a pointmap as input and either a georeference or a x/y extent. In the first case the georeference has to be defined
     * outside the application. The coordinates of the input points will be transformed to the coordinate system of the input georeference (which might
     * be different). The extent of the new raster will be the size of the georeference. If the second options is choosen the output raster will have the same
     * coordinate system as the input featurecoverage and an x and y size (in pixels) as defined by the parameters. In effect a new Corners georeference
     * will be created on the fly
    */
    operation.setSyntax("line2raster(inputlinecoverage,targetgeoref )");
    operation.setDescription(TR("translates a the points of a featurecoverage to pixels in a rastermap"));
    /*
     * so the expression has either 2 input parameters. A featurecoverage and georeference. Or 3 parameters, a featurecoverage, x extent (in pixels) and y extent (in pixels)
    */
    operation.setInParameterCount({2});
    /*
     * for the input type itLINE is specified. All coverages of the type itPOINT are featurecoverage and in effect the (aggregated) type itFEATURE is the
     * logical combination itFEATURE = itPOINT | itLINE | itPOLYGON
    */
    operation.addInParameter(0,itLINE, TR("input featurecoverage"),TR("input featurecoverage with any domain"));
    /*
     * There are two possible types of parameters here. It is either a georeference in the case of the 2 parameter variant or an integer for the x size
     * in the case of the three parameter variant
     */
    operation.addInParameter(1,itGEOREF, TR("input georeference"),TR("The georeference of the to be created rastermap"));
    /*
     * third parameter will only be there when the size of the input map will be given as an x/y pair
     */

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"), TR("output rastercoverage with the domain of the input map"));
    operation.setKeywords("raster,line,vector");
    mastercatalog()->addItems({operation});

    return operation.id();
}


