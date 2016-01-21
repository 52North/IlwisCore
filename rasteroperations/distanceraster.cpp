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



bool DistanceRaster::setDistanceValue(PixelIterator iter, PixelIterator neighbour, Size<> sz, double weight)
{
    double pixOrigValue = *iter;
    quint32 iterx = iter.x();
    quint32 itery = iter.y();
    quint32 iterz = iter.z();

    if (iterx > 0 && itery > 0)
        *iter = min(*iter, ( *neighbour[Pixel(iterx-1, itery-1, iterz)] + 7 ) * weight);
    if (iterx < sz.xsize()-1 && itery > 0)
        *iter = min(*iter, ( *neighbour[Pixel(iterx+1, itery-1, iterz)] + 7 ) * weight);
    if (iterx > 0 && itery < sz.ysize()-1)
        *iter = min(*iter, ( *neighbour[Pixel(iterx-1, itery+1, iterz)] + 7 ) * weight);
    if (iterx < sz.xsize()-1 && itery < sz.ysize()-1)
        *iter = min(*iter, ( *neighbour[Pixel(iterx+1, itery+1, iterz)] + 7 ) * weight);

    if (iterx > 0)
        *iter = min(*iter, ( *neighbour[Pixel(iterx-1, itery, iterz)] + 5 ) * weight);
    if (itery < sz.ysize()-1)
        *iter = min(*iter, ( *neighbour[Pixel(iterx, itery+1, iterz)] + 5 ) * weight);
    if (iterx < sz.xsize()-1)
        *iter = min(*iter, ( *neighbour[Pixel(iterx+1, itery, iterz)] + 5 ) * weight);
    if (itery > 0)
        *iter = min(*iter, ( *neighbour[Pixel(iterx, itery-1, iterz)] + 5 ) * weight);

    return pixOrigValue != *iter;
}

void DistanceRaster::distanceCalculation() {

    PixelIterator iter(_outputRaster);
    PixelIterator copyIter(_outputRaster); // hmm, find out how to reset an iterator....
    PixelIterator neighbour(_outputRaster);
    PixelIterator inpIter(_inputRaster);
    //PixelIterator weight(_inputWeightRaster);

    bool hasChanges = true; // the loop needs to start, so we set this as true...
    Size<> sz = _outputRaster->size();

    Envelope envelope = _outputRaster->envelope();
    double pixsizex = 1;

    if ( envelope.isValid()){
        pixsizex = envelope.xlength() / sz.xsize();
    }

    // copy input raster values to output raster
    while (inpIter != end(_inputRaster)) {
        *copyIter = *inpIter;
        ++inpIter;
        ++copyIter;
    }

    // All source pixels obtain distance value 0; all other pixels obtain a value distance value that is infinitely large.
    for( auto& value : _outputRaster) {
        if (value != rUNDEF && value != -1e+308)
            value = 0;
        else
            value = 1e100;
    }

    while (hasChanges) {
        hasChanges = false;
        while (iter != end(_outputRaster)) {
            //hasChanges |= setDistanceValue(iter, neighbour, sz, _hasWeightRaster ? *weight[Pixel(iter.x(), iter.y(), iter.z())] : 1.0);
            hasChanges |= setDistanceValue(iter, neighbour, sz, 1.0);
            ++iter;
        }

        if (hasChanges) {
            iter.end();
            hasChanges = false;

            while (iter != begin(_outputRaster)) {
                //hasChanges |= setDistanceValue(iter, neighbour, sz, _hasWeightRaster ? *weight[Pixel(iter.x(), iter.y(), iter.z())] : 1.0);
                hasChanges |= setDistanceValue(iter, neighbour, sz, 1.0);
                --iter;
            }
        }
    }

    // To obtain distance values in meters, these raw values are divided by 5 and
    // multiplied by the pixel size and a correction factor.
    for( auto& value : _outputRaster) {
        value *= pixsizex; // todo: check this!!
        value = value / 5 * 0.968;
    }
}


bool DistanceRaster::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    BoxedAsyncFunc distanceFun = [&](const BoundingBox& box) -> bool {
        distanceCalculation();
        return true;
    };

    bool resource = OperationHelperRaster::execute(ctx, distanceFun, _outputRaster);

    if ( resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputRaster);
        ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->source() );
    }
    return resource;
}

Ilwis::OperationImplementation *DistanceRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new DistanceRaster(metaid, expr);
}


Ilwis::OperationImplementation::State DistanceRaster::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString inputRasterName = _expression.input<QString>(0);
    QString inputWeightRaster = _expression.input<QString>(1);
    QString inputThiessenRaster = _expression.input<QString>(2);
    QString outputRasterName = _expression.parm(0,false).value();

    if (!_inputRaster.prepare(inputRasterName, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,inputRasterName,"");
        return sPREPAREFAILED;
    }

    _hasWeightRaster = false;
    if (0 != inputWeightRaster.length()) {
        if(!_inputWeightRaster.prepare(inputWeightRaster, itRASTER)) {
            ERROR2(ERR_COULD_NOT_LOAD_2,inputWeightRaster,"");
            return sPREPAREFAILED;
        } else {
            _hasWeightRaster = true;
        }
    }

    if (0 != inputThiessenRaster.length() && !_inputThiessenRaster.prepare(inputThiessenRaster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,inputThiessenRaster,"");
        return sPREPAREFAILED;
    }

    _outputRaster = OperationHelperRaster::initialize(_inputRaster, itRASTER, itCOORDSYSTEM | itENVELOPE | itGEOREF | itRASTERSIZE);

    IDomain dom;
    dom.prepare("value");
    _outputRaster->datadefRef() = DataDefinition(dom);

    for (int i = 0; i <_outputRaster->size().zsize(); i++)
        _outputRaster->setBandDefinition(i, dom);

    if (outputRasterName != sUNDEF){
        _outputRaster->name(outputRasterName);
    }
    ITable inputTbl = _inputRaster->attributeTable();

    if (inputTbl.isValid()) {
        _outputRaster->attributeTable(static_cast<Table *>(inputTbl->clone()));
    }

    return sPREPARED;
}


quint64 DistanceRaster::createMetadata()
{
    OperationResource operation({"ilwis://operations/distanceraster"});
    operation.setSyntax("distanceraster(raster, weightraster, thiessenraster)");
    operation.setDescription(TR("allows you to reduce the pixel size of a map"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itRASTER , TR("input raster"),TR("input rastermap"));
    operation.addInParameter(1,itRASTER , TR("weight raster"),TR("input weightraster"));
    operation.addInParameter(2,itRASTER , TR("thiessenmap"),TR("input Thiessen raster"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("output distance raster."));
    operation.setKeywords("distance, raster, map");

    mastercatalog()->addItems({operation});
    return operation.id();
}
