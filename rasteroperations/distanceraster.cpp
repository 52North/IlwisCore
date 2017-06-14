#include <functional>
#include <future>
#include <QString>
#include "kernel.h"
#include "raster.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "rasterinterpolator.h"
#include "distanceraster.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(DistanceRaster)

#define UNDEF -1e+308

DistanceRaster::DistanceRaster()
{

}

DistanceRaster::DistanceRaster(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr)
{

}

DistanceRaster::~DistanceRaster()
{

}

bool DistanceRaster::isValueMap() { return _inputRaster->datadef().domain<>()->ilwisType() == itNUMERICDOMAIN;}
bool DistanceRaster::isItemMap() { return _inputRaster->datadef().domain<>()->ilwisType() == itITEMDOMAIN;}

void DistanceRaster::ThiessenMapCalculation() {return;}

/*
 *
 *

7 5 7
5 0 5
7 5 7

 *
 */


double DistanceRaster::Min(double val1, double val2) {
    if (val1 == val2 && val2 == UNDEF)
        return UNDEF;
    else if (val2 == UNDEF)
        return val1;
    else if (val1 == UNDEF)
        return val2;
    else return min(val1, val2);

}

bool DistanceRaster::setDistanceValue(PixelIterator & iter, PixelIterator & neighbour, Size<> sz, double inputWeight, CalcDirection cd)
{
    double pixOrigValue = *iter;
    quint32 iterx = iter.x();
    quint32 itery = iter.y();
    quint32 iterz = iter.z();

    double weight = inputWeight == UNDEF ? 1e100 : inputWeight;

    // Algorithm functionality was kept as the way it was in Ilwis 3.8.5 (Scan-lines based)
    double &value = *iter;
    if (cd == CalcDirection::cdForward) {
        if (iterx > 0)
            value = Min(value,  *neighbour[Pixel(iterx-1, itery, iterz)] + (5 * weight));
        if (iterx < sz.xsize()-1)
            value = Min(value,  *neighbour[Pixel(iterx+1, itery, iterz)] + (5 * weight));
        if (itery > 0)
            value = Min(value,  *neighbour[Pixel(iterx, itery-1, iterz)] + (5 * weight));
        if (iterx > 0 && itery > 0)
            value = Min(value,  *neighbour[Pixel(iterx-1, itery-1, iterz)] + (7 * weight));
        if (iterx < sz.xsize()-1 && itery > 0)
            value = Min(value,  *neighbour[Pixel(iterx+1, itery-1, iterz)] + (7 * weight));
    } else {
        if (iterx < sz.xsize()-1)
            value = Min(value,  *neighbour[Pixel(iterx+1, itery, iterz)] + (5 * weight));
        if (iterx > 0)
            value = Min(value,  *neighbour[Pixel(iterx-1, itery, iterz)] + (5 * weight));
        if (itery < sz.ysize()-1)
            value = Min(value,  *neighbour[Pixel(iterx, itery+1, iterz)] + (5 * weight));
        if (iterx > 0 && itery < sz.ysize()-1)
            value = Min(value,  *neighbour[Pixel(iterx-1, itery+1, iterz)] + (7 * weight));
        if (iterx < sz.xsize()-1 && itery < sz.ysize()-1)
            value = Min(value,  *neighbour[Pixel(iterx+1, itery+1, iterz)] + (7 * weight));
    }

    return pixOrigValue != value;
}

void DistanceRaster::distanceCalculation() {

    PixelIterator iter(_outputRaster);
    PixelIterator copyIter(_outputRaster); // hmm, find out how to reset an iterator....

    PixelIterator neighbour(_outputRaster);
    PixelIterator inpIter(_inputRaster);
    PixelIterator weight;

    if(_hasWeightRaster) weight.setRaster(_inputOptWeightRaster);

    bool hasChanges = false;
    bool firstPass = true;
    Size<> sz = _outputRaster->size();
    double pixsize = _outputRaster->georeference()->pixelSize();

    quint64 currentCount = 0;

    initialize(_inputRaster->size().linearSize() );
    currentCount = 0;

    // copy input raster values to output raster
    // All source pixels obtain distance value 0; all other pixels obtain a value distance value that is infinitely large.
    while (inpIter != end(_inputRaster)) {

        if (*inpIter != UNDEF)
            *copyIter = 0;
        else if (_hasWeightRaster && *weight[Pixel(inpIter.x(), inpIter.y(), inpIter.z())] == UNDEF)
            *copyIter = UNDEF;
        else
            *copyIter = 1e100;

        ++inpIter;
        ++copyIter;

        updateTranquilizer(currentCount++, 1000);
    }


    // todo: handle properly maps with multiple layers (z > 0). Not tested with those yet, maybe needs some
    // proper handling when passing from one layer to another
    while (hasChanges || firstPass) {
        initialize(_inputRaster->size().linearSize() );
        currentCount = 0;

        hasChanges = false;

        for( iter = begin(_outputRaster); iter != end(_outputRaster); ++iter) {
            hasChanges |= setDistanceValue(iter, neighbour, sz, _hasWeightRaster ? *weight[Pixel(iter.x(), iter.y(), iter.z())] : 1.0, CalcDirection::cdForward );
            updateTranquilizer(currentCount++, 1000);
        }

        if (hasChanges || firstPass) {
            initialize(_inputRaster->size().linearSize() );
            currentCount = 0;

            iter.end();
            hasChanges = false;            

            while (iter != begin(_outputRaster)) {
                hasChanges |= setDistanceValue(iter, neighbour, sz, _hasWeightRaster ? *weight[Pixel(iter.x(), iter.y(), iter.z())] : 1.0, CalcDirection::cdBackward);
                --iter;
                updateTranquilizer(currentCount++, 1000);
            }
        }
        firstPass = false;
    }

    initialize(_inputRaster->size().linearSize() );
    currentCount = 0;

    // To obtain distance values in meters, these raw values are divided by 5 and
    // multiplied by the pixel size and a correction factor.
    for( auto& value : _outputRaster) {
        if (value >= 1e100) {
            value = UNDEF;
        } else {
            value *= pixsize / 5 * 0.968;
        }
        updateTranquilizer(currentCount++, 1000);
    }
}


bool DistanceRaster::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    if (_hasWeightRaster) {
        bool needGeoRefTransformation = !_inputRaster->georeference()->isCompatible(_inputOptWeightRaster->georeference());
        if (needGeoRefTransformation) {
            if (!OperationHelperRaster::resample(_inputRaster, _inputOptWeightRaster, ctx)) {
                return ERROR2(ERR_COULD_NOT_CONVERT_2, TR("georeferences"), TR("common base"));
            }
        }
    }

    BoxedAsyncFunc distanceFun = [&](const BoundingBox& box) -> bool {
        distanceCalculation();
        return true;
    };

    bool resource = OperationHelperRaster::execute(ctx, distanceFun, _outputRaster);

    if ( resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputRaster);
        logOperation(_outputRaster,_expression);
        ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource() );
    }
    return resource;
}

Ilwis::OperationImplementation *DistanceRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new DistanceRaster(metaid, expr);
}

Ilwis::OperationImplementation::State DistanceRaster::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString inputRasterName = _expression.input<QString>(0);
    //QString inputThiessenRaster = _expression.input<QString>(2);
    QString outputRasterName = _expression.parm(0,false).value();

    if (!_inputRaster.prepare(inputRasterName, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,inputRasterName,"");
        return sPREPAREFAILED;
    }

    if (_expression.parameterCount() == 2) {
        _hasWeightRaster = true;
        QString inputWeightRaster = _expression.input<QString>(1);
        if(!_inputOptWeightRaster.prepare(inputWeightRaster, itRASTER)) {
            ERROR2(ERR_COULD_NOT_LOAD_2,inputWeightRaster,"");
            return sPREPAREFAILED;
        }
    } else {
        _hasWeightRaster = false;
    }

    /*if (0 != inputThiessenRaster.length() && !_inputThiessenRaster.prepare(inputThiessenRaster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,inputThiessenRaster,"");
        return sPREPAREFAILED;
    }*/

    _outputRaster = OperationHelperRaster::initialize(_inputRaster, itRASTER, itCOORDSYSTEM | itENVELOPE | itGEOREF | itRASTERSIZE);  

    IDomain dom;
    dom.prepare("value");

    _outputRaster->datadefRef() = DataDefinition(dom);    

    for (int i = 0; i <_outputRaster->size().zsize(); i++)
        _outputRaster->setBandDefinition(i, dom);

    if (outputRasterName != sUNDEF){
        _outputRaster->name(outputRasterName);
    }

    return sPREPARED;
}


quint64 DistanceRaster::createMetadata()
{
    OperationResource operation({"ilwis://operations/distanceraster"});
    operation.setSyntax("distanceraster(raster, weightraster)");
    operation.setDescription(TR("calculate raster map distances"));
    operation.setInParameterCount({1,2});
    operation.addInParameter(0,itRASTER , TR("input raster"),TR("input rastermap"));
    operation.addOptionalInParameter(1,itRASTER , TR("weight raster"),TR("input weightraster"));
    //operation.addInParameter(2,itRASTER , TR("thiessenmap"),TR("input Thiessen raster"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("output distance raster."));
    operation.setKeywords("raster,interpolation");

    mastercatalog()->addItems({operation});
    return operation.id();
}
