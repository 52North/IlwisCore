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

static const char * sWeightFunc[] = { "InvDist", "Linear", 0 };

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

static int iFind(const std::string& s, const char* sArr[])
{
      int i = 0;
      while (sArr[i]) {
        if (strncasecmp(sArr[i], s.c_str(), s.length()) == 0)
          return i;
        i++;
      }
      return shUNDEF;
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
    double rLimD = limDist; //local substit

    double rMinDist = limDist * 1.0e-10; // minimal distance taken into account
    double rLimDist2 = rLimD;

    iNrPoints = _inputfeatures->featureCount(itPOINT);
    iNrValidPnts = 0;   // valid point counter
    cPoints.resize(iNrPoints);
    rAttrib.resize(iNrPoints);
    for(auto feature :  _inputfeatures){
        if ( feature->geometryType() != itPOINT)
            continue;
        double rAtr;
        Coordinate crd(*feature->geometry()->getCoordinate());
        QVariant d = _inputfeatures->coord2value(crd);
        if (d.isValid() && crd != crdUNDEF){
           QVariantMap vmap = d.value<QVariantMap>();
           QVariant attribute =  vmap[COVERAGEKEYCOLUMN];
           rAtr = attribute.toDouble();
        }else{
           rAtr = -1;
        }
        if ( _needCoordinateTransformation){
            crd = _outputraster->coordinateSystem()->coord2coord(_inputfeatures->coordinateSystem(), crd);
            if (crd == crdUNDEF)
                continue;
        }
        cPoints[iNrValidPnts] = crd;
        rAttrib[iNrValidPnts] = rAtr;
        ++iNrValidPnts;
    }

    PixelIterator pixiter(_outputraster);
    Pixel pix;
    for (long ypos = 0; ypos < ysize; ypos++)  {
        pix.y = ypos;
        for (long xpos = 0; xpos < xsize; xpos++)  {
            pix.x = xpos;
            Coordinate crdRC = _inputgrf->pixel2Coord(pix);
            double rSumW   = 0.0;     // Sum Weight's
            double rSumWtA = 0.0;     // Sum of Weight times atrribute
            double rW;
            double rDis2;
            for (long iPc = 0; iPc < iNrValidPnts; iPc++)  {
                rDis2 = crdRC.distance(cPoints[iPc]);
                /*if (rDis2 < rMinDist && wft == wfEXACT ) {
                    rSumWtA = rAttrib[iPc];  // only 1 point exactly in center of estimated
                    rSumW = 1;// pixel contributes to its value
                    break;                   // no more points, even if they
                }else*/
                if (rDis2 < rLimDist2)  {
                   if(wft == wfEXACT){
                     rW = rInvDist(rDis2);
                   }
                   else{
                     rW = rLinDecr(rDis2);
                   }
                   rSumW   += rW;
                   rSumWtA += rW * rAttrib[iPc];
                }
            }
            double rV;
            if (rSumW == 0.0){
                rV = rUNDEF;
            }
            else{
                 rV = rSumWtA / rSumW;  // apply normalized weights
            }
            pixiter = pix;
            *pixiter = rV;
        }

    }
    QVariant value;
    value.setValue<IFeatureCoverage>(_inputfeatures);
    ctx->setOutput(symTable,value,_outputraster->name(), itRASTER, _outputraster->source() );
    return true;
}


Ilwis::OperationImplementation::State MovingAverage::prepare(ExecutionContext *ctx, const SymbolTable &)
{

    QString features = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputfeatures.prepare(features, itFEATURE)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,features,"");
        return sPREPAREFAILED;
    }
    if ( _expression.parameterCount() == 5){ // the georef case         
        QString georefname = _expression.parm(4).value();
        if (!_inputgrf.prepare(georefname, itGEOREF)) {
            ERROR2(ERR_COULD_NOT_LOAD_2,georefname,"");
            return sPREPAREFAILED;
        }
        weightFunct = _expression.parm(1).value();
        wft = (WeightFuncType)iFind(weightFunct.toStdString(), sWeightFunc);

        if(wft == shUNDEF){
            return sPREPAREFAILED;
        }

        exp = _expression.parm(2).value().toDouble();
        limDist = _expression.parm(3).value().toDouble();

    }else if ( _expression.parameterCount() == 6){

        int xsize = _expression.parm(4).value().toInt();
        int ysize = _expression.parm(5).value().toInt();

        IGeoReference grf(outputName);
        grf->create("corners");
        grf->size(Size<>(xsize, ysize,1));
        grf->coordinateSystem(_inputfeatures->coordinateSystem());
        grf->envelope(_inputfeatures->envelope());
        grf->compute();
        _inputgrf = grf;
        wft = wfEXACT;
        weightFunct = _expression.parm(1).value();
        if ( weightFunct.toLower() != "invdist")
             wft = wfNOTEXACT;
        exp = _expression.parm(2).value().toDouble();
        limDist = _expression.parm(3).value().toDouble();
    }
    _needCoordinateTransformation = _inputgrf->coordinateSystem() != _inputfeatures->coordinateSystem();
    IDomain dom("code=value");
    _outputraster = IRasterCoverage(outputName);
    _outputraster->datadefRef().domain(dom);
    _outputraster->coordinateSystem(_inputgrf->coordinateSystem());
    Envelope env = _inputgrf->coordinateSystem()->convertEnvelope(_inputfeatures->coordinateSystem(), _inputfeatures->envelope());
    _outputraster->envelope(env);
    _outputraster->georeference(_inputgrf);

    if (limDist == rUNDEF || limDist < EPS10){

        return sPREPAREFAILED;
    }
    return sPREPARED;
}

double MovingAverage::rInvDist(double rDis)
{
    double rLimit = limDist;
    if ((rDis < EPS20) || (rDis > rLimit))
        return 0;
    double rX = rLimit / rDis ; // reduced distance inverted
    return std::pow(std::abs(rX), exp) - 1; // w = (1/d)^n - 1
}

double MovingAverage::rLinDecr(double rDis)
{
      double rLimit = limDist;
      if (rDis < EPS10)
        return 1;
      if (rDis > rLimit)
        return 0;
      double rX = rDis / rLimit; // reduced distance
      return 1 - std::pow(std::abs(rX), exp); // w = 1 - d^n
}

quint64 MovingAverage::createMetadata()
{

    OperationResource operation({"ilwis://operations/movingaverage"});
    operation.setLongName("Moving Average");
    operation.setSyntax("movingaverage(inputpointmap,invDist | linear,exp,limDist,georef| xsize[,ysize])");
    operation.setDescription(TR("The Moving average operation is a point interpolation which requires a point map as input and returns a raster map as output"));
    operation.setInParameterCount({5,6});
    operation.addInParameter(0,itPOINT, TR("input featurecoverage"),TR("input featurecoverage with any domain"));
    operation.addInParameter(1,itSTRING,TR("weight function"),TR("The method of weight function to be applied. Either Inverse Distance method or Linear distance method"));
    operation.addInParameter(2,itDOUBLE, TR("weight exponent"),TR("value for weight exponent n to be used in the specified weight function (real value, usually a value close to 1.0)."));
    operation.addInParameter(3,itDOUBLE,TR("Limiting distance"),TR("value for the limiting distance: points that are farther away from an output pixel than the limiting distance obtain weight zero"));
    operation.addInParameter(4,itGEOREF | itINTEGER, TR("input georeference or x size"),TR("the parameter can either be a georeference or the x extent of the the to be created raster"));
    operation.addInParameter(5,itINTEGER, TR("input y size"),TR("optional y size of the output raster. Only used of the previous parameter was an x size"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"), TR("output rastercoverage with the domain of the input map"));
    operation.setKeywords("raster,pointmap");
    mastercatalog()->addItems({operation});
    return operation.id();
}



