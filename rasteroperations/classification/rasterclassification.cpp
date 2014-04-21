#include <functional>
#include <future>
#include <unordered_map>
#include "kernel.h"
#include "raster.h"
#include "itemdomain.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "classification/sampleset.h"
#include "classifier.h"
#include "rasterclassification.h"

using namespace Ilwis;
using namespace RasterOperations;

RasterClassification::RasterClassification()
{
}

RasterClassification::RasterClassification(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool RasterClassification::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    PixelIterator iterIn(_sampleSet.sampleRasterSet());
    iterIn.setFlow(PixelIterator::fZXY);

    PixelIterator iterOut(_outputRaster);

    _classifier->classify(iterIn, iterOut);

    return true;

}

Ilwis::OperationImplementation::State RasterClassification::prepare(ExecutionContext *, const SymbolTable &sym){
    QString samplesetName = _expression.parm(0).value();
    _sampleSet = mastercatalog()->name2Resource(samplesetName);
    if ( !_sampleSet.isValid()){
        _sampleSet = sym.getValue(samplesetName).value<SampleSet>();
        if (!_sampleSet.isValid()){
            ERROR2(ERR_COULD_NOT_LOAD_2,"SampleSet", samplesetName);
            return sPREPAREFAILED;
        }
    }
    QString outputName = _expression.parm(0,false).value();

    _outputRaster = OperationHelperRaster::initialize(_sampleSet.sampleRasterSet(),itRASTER, itCOORDSYSTEM | itGEODETICDATUM);
   if ( !_outputRaster.isValid()) {
       ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
       return sPREPAREFAILED;
   }
   _outputRaster->datadef().domain(_sampleSet.thematicDomain());
   _outputRaster->size(_sampleSet.sampleRasterSet()->size().twod());
   if ( outputName!= sUNDEF)
       _outputRaster->name(outputName);


    return sPREPARED;
}
//-------------------------------------------------------

REGISTER_OPERATION(BoxClassification)

BoxClassification::BoxClassification(quint64 metaid, const Ilwis::OperationExpression &expr) : RasterClassification(metaid, expr)
{

}

Ilwis::OperationImplementation *BoxClassification::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new BoxClassification(metaid, expr);
}

Ilwis::OperationImplementation::State BoxClassification::prepare(ExecutionContext *ctx, const SymbolTable &sym)
{
    OperationImplementation::State prepareState = sNOTPREPARED;

    if ( (prepareState = RasterClassification::prepare(ctx,sym)) != sPREPARED){
        return prepareState;
    }

    bool ok;
    _widenFactor = _expression.parm(1).value().toDouble(&ok);
    if (!ok || _widenFactor <= 0){
        ERROR2(ERR_ILLEGAL_VALUE_2, "widen factor", _expression.parm(1).value());
    }

    _classifier.reset( new BoxClassifier(_widenFactor,_sampleSet));

    return sNOTPREPARED;
}

quint64 BoxClassification::createMetadata()
{
    OperationResource operation({"ilwis://operations/boxclassifcation"});
    operation.setSyntax("boxclassifcation(samplesetname,widen-factor)");
    operation.setDescription(TR("performs a multi-spectral image classification according to training pixels in a sample set"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itSAMPLESET , TR("Sampleset"),TR("Training set of pixels"));
    operation.addInParameter(1,itDOUBLE , TR("widen-factor"),TR("allows you to widen (factor > 1) the boxes that are 'drawn' around class means"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(1,itRASTER, TR("output rastercoverage with the domain of the sampleset"));
    operation.setKeywords("classification,raster");

    mastercatalog()->addItems({operation});
    return operation.id();
}

