#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "blockiterator.h"
#include "pixeliterator.h"
#include "rasterfilter.h"
#include "linearrasterfilter.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(LinearRasterFilter)

LinearRasterFilter::LinearRasterFilter()
{
}


LinearRasterFilter::LinearRasterFilter(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool LinearRasterFilter::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

   BoxedAsyncFunc filterFun = [&](const BoundingBox& box) -> bool {
        PixelIterator iterOut(_outputRaster, box);
        BlockIterator blockIter(_inputRaster,_filter->size(), box, Size<>(1,1,1));
        PixelIterator iterEnd = iterOut.end();
        while(iterOut != iterEnd) {
            *iterOut = _filter->applyTo(*blockIter);
            ++iterOut;
            ++blockIter;
        }
        return true;
    };

    bool res = OperationHelperRaster::execute(ctx, filterFun, _outputRaster);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputRaster);
        ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->source() );
    }
    return res;

}

Ilwis::OperationImplementation *LinearRasterFilter::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new LinearRasterFilter(metaid, expr);
}

Ilwis::OperationImplementation::State LinearRasterFilter::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString raster1 = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputRaster.prepare(raster1, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster1,"");
        return sPREPAREFAILED;
    }
    QString expr = _expression.parm(1).value();
    int copylist = itDOMAIN | itCOORDSYSTEM | itGEOREF;
    _outputRaster = OperationHelperRaster::initialize(_inputRaster.as<IlwisObject>(),itRASTER, copylist);
    if ( !_outputRaster.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }
    if ( outputName != sUNDEF)
        _outputRaster->name(outputName);
    _filter.reset(new LinearGridFilter(expr));
    if ( !_filter->isValid())
        return sPREPAREFAILED;

    return sPREPARED;
}

quint64 LinearRasterFilter::createMetadata()
{
    OperationResource operation({"ilwis://operations/linearrasterfilter"});
    operation.setSyntax("linearrasterfilter(raster1, filterdefinition=linearfiltername | expression");
    operation.setDescription(TR("generates a new raster based on the conditions as specified in the linear filter definition"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER , TR("rastercoverage"),TR("input rastercoverage with numeric domain"));
    operation.addInParameter(1,itSTRING , TR("filter definition"),TR("Definition of the filter. This can be a predefined filter name or an expression defining the filter"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itTABLE, TR("output raster"),TR("output table with a numeric domain"));
    operation.setKeywords("filter,raster,numeric");

    mastercatalog()->addItems({operation});
    return operation.id();
}
