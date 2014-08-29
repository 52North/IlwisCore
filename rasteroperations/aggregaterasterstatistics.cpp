#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "operationhelpergrid.h"
#include "geometryhelper.h"
#include "aggregaterasterstatistics.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(AggregateRasterStatistics)

AggregateRasterStatistics::AggregateRasterStatistics()
{
}

bool AggregateRasterStatistics::execute(ExecutionContext *ctx, SymbolTable &symTable)
{

}

Ilwis::OperationImplementation *AggregateRasterStatistics::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{

}

Ilwis::OperationImplementation::State AggregateRasterStatistics::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    try{
    OperationHelper::check([&] ()->bool { return _inputRaster.prepare(_expression.input<QString>(0), itRASTER); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

    std::vector<QString> operations = {"mean","variance","standarddev","totalsumsquares","absolutedeviation","skew","kurtosis","max","min","maxindex","minindex","median"};

    OperationHelper::check([&] ()->bool { return std::find(operations.begin(), operations.end(),_expression.input<QString>(1)) != operations.end(); },
    {ERR_ILLEGAL_VALUE_2,TR("statistical operation"),_expression.input<QString>(1) } );

    _operation = _expression.input<QString>(1);

    if ( _expression.parameterCount() == 3){
        OperationHelper::check([&] ()->bool { return _relativeFromRaster.prepare(_expression.input<QString>(2), itRASTER); },
        {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(2), "" } );

        if(!_relativeFromRaster->georeference()->isCompatible(_inputRaster->georeference())){
            ERROR2(ERR_NOT_COMPATIBLE2,"georeference", QString("georeference of ").arg(_inputRaster->name()));
            return sPREPAREFAILED;
        }

    }

    return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 AggregateRasterStatistics::createMetadata()
{
    OperationResource operation({"ilwis://operations/aggregaterasterstatistics"});
    operation.setSyntax("aggregaterasterstatistics(inputraster,mean|variance|standarddev|totalsumsquares|absolutedeviation|skew|kurtosis|max|min|maxindex|minindex|median[,relativefromraster])");
    operation.setDescription(TR("transpose the raster according to the method indicated by the second parameter"));
    operation.setInParameterCount({2,3});
    operation.addInParameter(0,itRASTER,  TR("input raster"),TR("set raster bands to be aggregated"));
    operation.addInParameter(1,itSTRING, TR("statistical method"),TR("method of calucaltion for a pixel column of the stack of bands"));
    operation.addInParameter(2,itRASTER, TR("relative raster"),TR("calculations are made relative to the given raster, optional"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"), TR("Single band raster with the aggregated statical values"));
    operation.setKeywords("raster, geometry");

    mastercatalog()->addItems({operation});
    return operation.id();
}
