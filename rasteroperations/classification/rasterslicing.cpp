#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "itemdomain.h"
#include "interval.h"
#include "intervalrange.h"
#include "rasterslicing.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(RasterSlicing)

RasterSlicing::RasterSlicing()
{
}


RasterSlicing::RasterSlicing(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{

}

bool RasterSlicing::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    BoxedAsyncFunc sliceFun = [&](const BoundingBox& box) -> bool {
        PixelIterator iterOut(_outputRaster, box);
        PixelIterator iterEnd = iterOut.end();
        PixelIterator iterIn(_inputRaster, box);
        while(iterOut != iterEnd){
            double v = *iterIn;
            double &outValue = *iterOut;
            long i1 = _bounds.size();
            if (v == rUNDEF || v > _bounds[i1]){
                outValue = rUNDEF;
            } else {
                long i0 = 0;
                while (i0 < i1) {
                    long reduced = i0/2 + i1/2;
                    if (v <= _bounds[reduced])
                        i1 = reduced;
                    else
                        i0 = reduced + 1;
                }
                outValue = i0;
            }
            ++iterIn;
            ++iterOut;
        }
        return true;

    };

    bool res = OperationHelperRaster::execute(ctx, sliceFun, _outputRaster);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputRaster);
        ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->source() );
    }
    return res;

}

OperationImplementation *RasterSlicing::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RasterSlicing(metaid, expr);
}

OperationImplementation::State RasterSlicing::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString raster = _expression.parm(0).value();

    QString outputName = _expression.parm(0,false).value();

    if (!_inputRaster.prepare(raster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    QString domainName = _expression.parm(1).value();
    if (!_numericItems.prepare(domainName)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,domainName,"");
        return sPREPAREFAILED;
    }
    _bounds.resize(_numericItems->count(), 0);
    int  i =0;
    for(auto& v : _bounds){
        v = _numericItems->item(i++)->as<Interval>()->range().min();
    }

    IIlwisObject outputObj = OperationHelperRaster::initialize(_inputRaster,itRASTER, itCOORDSYSTEM | itGEOREF);
    if ( !outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }
    _outputRaster = outputObj.as<RasterCoverage>();
    _outputRaster->name(outputName);

    return sPREPARED;
}

quint64 RasterSlicing::createMetadata()
{
    OperationResource operation({"ilwis://operations/sliceraster"});
    operation.setSyntax("sliceraster(inputgridcoverage, IntervalDomain)");
    operation.setDescription(TR("Ranges of values of the input map are grouped together into one output class"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER , TR("input rastercoverage"),TR("input rastercoverage with domain value"));
    operation.addInParameter(1,itITEMDOMAIN , TR("Numeric Item domain"),TR("The domain that used for the classification"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("classified raster"));
    operation.setKeywords("raster, classification");

    mastercatalog()->addItems({operation});
    return operation.id();
}
