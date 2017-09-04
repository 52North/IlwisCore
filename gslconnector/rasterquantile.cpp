#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "operationhelpergrid.h"
#include "geometryhelper.h"
#include "rasterquantile.h"
#include "gsl/gsl_statistics.h"

using namespace Ilwis;
using namespace GSL;

REGISTER_OPERATION(RasterQuantile)

RasterQuantile::RasterQuantile()
{
}

RasterQuantile::RasterQuantile(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool RasterQuantile::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    double inValue;
    bool xchanged = false;
    std::vector<double> zcolumn;
    zcolumn.reserve(_inputRaster->size().zsize());
    PixelIterator iterIn(_inputRaster, BoundingBox(),PixelIterator::fZXY);
    //PixelIterator iterIn(_inputRaster);
    for(auto& value : _outputRaster){
        while(!xchanged){
            if(( inValue = *iterIn) != rUNDEF)
                zcolumn.push_back(inValue);
            ++iterIn;
            xchanged = iterIn.xchanged();
        }
        value = rUNDEF;
        std::sort(zcolumn.begin(), zcolumn.end());
        if ( zcolumn.size() > 0){
            value = gsl_stats_quantile_from_sorted_data(&zcolumn[0],1,zcolumn.size(),_quantile);
        }
        zcolumn.clear(); // next column
        xchanged = false; // reset flag as we are in the next column now
    }
    QVariant value;
    value.setValue<IRasterCoverage>(_outputRaster);
    logOperation(_outputRaster,_expression);
    ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource() );
    return true;
}

Ilwis::OperationImplementation *RasterQuantile::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RasterQuantile(metaid,expr);
}

Ilwis::OperationImplementation::State RasterQuantile::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        OperationHelper::check([&] ()->bool { return _inputRaster.prepare(_expression.input<QString>(0), itRASTER); },
        {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

        quint8 quant = _expression.input<quint8>(1);
        OperationHelper::check([&] ()->bool {  return quant > 0 && quant < 100; },
        {ERR_ILLEGAL_VALUE_2,TR("Illegal quantile number"), QString(TR("must be between 0 and 100, found : %1").arg(quant)) } );

        _quantile = (double)quant / 100.0;

        QString outputName = _expression.parm(0,false).value();

        OperationHelperRaster::initialize(_inputRaster, _outputRaster, itCOORDSYSTEM | itGEODETICDATUM | itGEOREF);
        if ( !_outputRaster.isValid()) {
            ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
            return sPREPAREFAILED;
        }
        _outputRaster->datadefRef().domain(IDomain("value"));
        _outputRaster->size(_inputRaster->size().twod());
        if ( outputName!= sUNDEF)
            _outputRaster->name(outputName);




        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 RasterQuantile::createMetadata()
{
    OperationResource operation({"ilwis://operations/rasterquantile"},"gsl");
    operation.setSyntax("rasterquantile(inputraster, quantile)");
    operation.setDescription(TR("calculates a raster with the quantile value of the sorted z columns of a raster coverage"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER,  TR("input raster"),TR("set raster bands to be aggregated"));
    operation.addInParameter(1,itPOSITIVEINTEGER, TR("quantile"),TR("quantile of the ordered z column"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"), TR("Single band raster with the aggregated statical values"));
    operation.setKeywords("raster, statistics,numeric");

    mastercatalog()->addItems({operation});
    return operation.id();
}
