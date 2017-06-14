#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "featurecoverage.h"
#include "feature.h"
#include "table.h"
#include "featureiterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "movingaverage.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(MovingAverage)

MovingAverage::MovingAverage()
{

}

MovingAverage::~MovingAverage()
{

}

MovingAverage::MovingAverage(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr)
{

}

Ilwis::OperationImplementation *MovingAverage::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MovingAverage(metaid,expr);
}

bool MovingAverage::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    std::vector<Ilwis::Coordinate> cPoints;
    std::vector<double> rAttrib;
    long xsize = _inputgrf->size().xsize();
    long ysize = _inputgrf->size().ysize();

    double rMinDist = _limDist * 1.0e-10; // minimal distance taken into account

    long iNrPoints = _inputfeatures->featureCount(itPOINT);
    long iNrValidPnts = 0;   // valid point counter
    cPoints.resize(iNrPoints);
    rAttrib.resize(iNrPoints);
    bool needCoordinateTransformation = _inputgrf->coordinateSystem() != _inputfeatures->coordinateSystem();
    for(auto feature :  _inputfeatures){
        if ( feature->geometryType() != itPOINT)
            continue;
        Coordinate crd(*feature->geometry()->getCoordinate());
        if ( needCoordinateTransformation)
            crd = _outputraster->coordinateSystem()->coord2coord(_inputfeatures->coordinateSystem(), crd);
        if (crd.isValid()) {
            double rVal = feature(_attribute).toDouble();
            cPoints[iNrValidPnts] = crd;
            rAttrib[iNrValidPnts] = rVal;
            ++iNrValidPnts;
        }
    }

    PixelIterator pixiter(_outputraster);
    Pixel pix;
    double mmax = -1e308, mmin = 1e308;
    for (long ypos = 0; ypos < ysize; ypos++)  {
        pix.y = ypos;
        for (long xpos = 0; xpos < xsize; xpos++)  {
            pix.x = xpos;
            if (pix.isValid() && pixiter.contains(pix)) {
                Coordinate crdRC = _inputgrf->pixel2Coord(pix);
                double rSumW   = 0.0;     // Sum Weight's
                double rSumWtA = 0.0;     // Sum of Weight times atrribute
                for (long iPc = 0; iPc < iNrValidPnts; iPc++)  {
                    double rDistance = crdRC.distance(cPoints[iPc]);
                    if (rDistance < rMinDist && _wft == wfEXACT ) {
                        rSumWtA = rAttrib[iPc];  // only 1 point exactly in center of estimated
                        rSumW = 1;               // pixel contributes to its value
                        break;                   // no more points, even if they coincide with this first-found point
                    } else if (rDistance < _limDist)  {
                        double rW;
                        if(_wft == wfEXACT) {
                            rW = rInvDist(rDistance);
                        } else {
                            rW = rLinDecr(rDistance);
                        }
                        rSumW   += rW;
                        rSumWtA += rW * rAttrib[iPc];
                    }
                }
                double value;
                if (rSumW == 0.0) {
                    value = rUNDEF;
                } else {
                    value = rSumWtA / rSumW;  // apply normalized weights
                    mmax = Ilwis::max(mmax, value);
                    mmin = Ilwis::min(mmin, value);
                }
                pixiter = pix;
                *pixiter = value;
            }
        }
    }

    NumericRange *rng = new NumericRange(mmin, mmax, 0);
    _outputraster->datadefRef().range(rng);
    QVariant value;
    value.setValue<IRasterCoverage>( _outputraster );
    logOperation(_outputraster,_expression);
    ctx->setOutput(symTable,value,_outputraster->name(), itRASTER, _outputraster->resource() );
    return true;
}

Ilwis::OperationImplementation::State MovingAverage::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString features = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputfeatures.prepare(features, itFEATURE)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,features,"");
        return sPREPAREFAILED;
    }
    _attribute = _expression.parm(1).value();
    QString weightFunct = _expression.parm(2).value();
    if ( weightFunct.toLower() == "invdist")
        _wft = wfEXACT;
    else if (weightFunct.toLower() == "linear")
        _wft = wfNOTEXACT;
    else {
        ERROR1("Invalid weight function '%1'; should be Linear or InvDist", weightFunct);
        return sPREPAREFAILED;
    }
    _exp = _expression.parm(3).value().toDouble();
    _limDist = _expression.parm(4).value().toDouble();
    if (_limDist == rUNDEF || _limDist < EPS10) {
        ERROR1("Invalid limiting distance '%1'", _expression.parm(4).value());
        return sPREPAREFAILED;
    }
    if ( _expression.parameterCount() == 6) { // the georef case
        QString georefname = _expression.parm(5).value();
        if (!_inputgrf.prepare(georefname, itGEOREF)) {
            ERROR2(ERR_COULD_NOT_LOAD_2,georefname,"");
            return sPREPAREFAILED;
        }
    } else if ( _expression.parameterCount() == 7) {
        int xsize = _expression.parm(5).value().toInt();
        int ysize = _expression.parm(6).value().toInt();
        IGeoReference grf(outputName);
        grf->create("corners");
        grf->size(Size<>(xsize, ysize,1));
        grf->coordinateSystem(_inputfeatures->coordinateSystem());
        grf->envelope(_inputfeatures->envelope());
        grf->compute();
        _inputgrf = grf;
    }
    quint32 colIndex = _inputfeatures->attributeDefinitions().columnIndex(_attribute);
    if (colIndex == iUNDEF) {
        ERROR2(ERR_COLUMN_MISSING_2,_attribute,features);
        return sPREPAREFAILED;
    }
    const DataDefinition & datadef = _inputfeatures->attributeDefinitions().columndefinitionRef(colIndex).datadef();
    IDomain dom;
    if (datadef.isValid())
        dom = datadef.domain();
    else
        dom = IDomain("code=domain:value");

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

double MovingAverage::rInvDist(double rDis)
{
    if ((rDis < EPS20) || (rDis > _limDist))
        return 0;
    double rX = _limDist / rDis ; // reduced distance inverted
    return std::pow(std::abs(rX), _exp) - 1; // w = (1/d)^n - 1
}

double MovingAverage::rLinDecr(double rDis)
{
    if (rDis < EPS10)
        return 1;
    if (rDis > _limDist)
        return 0;
    double rX = rDis / _limDist; // reduced distance
    return 1 - std::pow(std::abs(rX), _exp); // w = 1 - d^n
}

quint64 MovingAverage::createMetadata()
{
    OperationResource operation({"ilwis://operations/movingaverage"});
    operation.setLongName("Moving Average");
    operation.setSyntax("movingaverage(inputpointmap,attribute,invDist | linear,exp,limDist,georef| xsize[,ysize])");
    operation.setDescription(TR("The Moving average operation is a point interpolation which requires a point map as input and returns a raster map as output"));
    operation.setInParameterCount({6,7});
    operation.addInParameter(0,itPOINT, TR("input featurecoverage"),TR("input featurecoverage with any domain"));
    operation.addInParameter(1,itSTRING,TR("attribute"),TR("The attribute of the featurecoverage whose values are interpolated"));
    operation.addInParameter(2,itSTRING,TR("weight function"),TR("The method of weight function to be applied. Either Inverse Distance method or Linear distance method"));
    operation.addInParameter(3,itDOUBLE, TR("weight exponent"),TR("value for weight exponent n to be used in the specified weight function (real value, usually a value close to 1.0)."));
    operation.addInParameter(4,itDOUBLE,TR("Limiting distance"),TR("value for the limiting distance: points that are farther away from an output pixel than the limiting distance obtain weight zero"));
    operation.addInParameter(5,itGEOREF | itINTEGER, TR("input georeference or x size"),TR("the parameter can either be a georeference or the x extent of the the to be created raster"));
    operation.addInParameter(6,itINTEGER, TR("input y size"),TR("optional y size of the output raster. Only used of the previous parameter was an x size"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"), TR("output rastercoverage with the domain of the input map"));
    operation.setKeywords("raster,point,interpolation");
    mastercatalog()->addItems({operation});
    return operation.id();
}
