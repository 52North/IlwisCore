#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "operationhelpergrid.h"
#include "geometryhelper.h"
#include "relativeaggregaterasterstatistics.h"
#include "gsl/gsl_statistics.h"

using namespace Ilwis;
using namespace GSL;

REGISTER_OPERATION(RelativeAggregateRasterStatistics)

RelativeAggregateRasterStatistics::RelativeAggregateRasterStatistics()
{
}

RelativeAggregateRasterStatistics::RelativeAggregateRasterStatistics(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

double stats_min_m(const double data[], size_t stride, size_t n, double mean){
    return gsl_stats_min(data,1,n) - mean;
}

double stats_max_m(const double data[], size_t stride, size_t n, double mean){
    return mean - gsl_stats_max(data,1,n);
}

bool RelativeAggregateRasterStatistics::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    double inValue;
    bool xchanged = false;
    std::vector<double> zcolumn;
    zcolumn.reserve(_inputRaster->size().zsize());
    PixelIterator iterIn(_inputRaster, BoundingBox(),PixelIterator::fZXY);
    PixelIterator iterRelative(_relativeFromRaster);
    for(auto& value : _outputRaster){
        while(!xchanged){
            if(( inValue = *iterIn) != rUNDEF)
                zcolumn.push_back(inValue);
            ++iterIn;
            xchanged = iterIn.xchanged();
        }
        double relativeValue = *iterRelative;
        value = zcolumn.size() > 0 && relativeValue != rUNDEF ? _statisticsFunction(&zcolumn[0],1,zcolumn.size(),relativeValue) : rUNDEF;
        zcolumn.clear(); // next column
        xchanged = false; // reset flag as we are in the next column now
        ++iterRelative;
    }
    QVariant value;
    value.setValue<IRasterCoverage>(_outputRaster);
    logOperation(_outputRaster,_expression);
    ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource() );
    return true;
}

Ilwis::OperationImplementation *RelativeAggregateRasterStatistics::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RelativeAggregateRasterStatistics(metaid,expr);
}

Ilwis::OperationImplementation::State RelativeAggregateRasterStatistics::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        OperationHelper::check([&] ()->bool { return _inputRaster.prepare(_expression.input<QString>(0), itRASTER); },
        {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

        std::vector<QString> operations = {"variance","standarddev","totalsumsquares","absolutedeviation",
                                           "max","min","autocorrelationlag1","variancefixedmean","standarddevfixedmean"};

        OperationHelper::check([&] ()->bool { return std::find(operations.begin(), operations.end(),_expression.input<QString>(1)) != operations.end(); },
        {ERR_ILLEGAL_VALUE_2,TR("statistical operation"),_expression.input<QString>(1) } );

        _operationName = _expression.input<QString>(1);

        QString outputName = _expression.parm(0,false).value();

        OperationHelperRaster::initialize(_inputRaster, _outputRaster, itCOORDSYSTEM | itGEODETICDATUM | itGEOREF);
        if ( !_outputRaster.isValid()) {
            ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
            return sPREPAREFAILED;
        }
        if ( _operationName == "median" || _operationName == "min" || _operationName == "max")
            _outputRaster->datadefRef() = DataDefinition(_inputRaster->datadef().domain(),_inputRaster->datadef().range()->clone());
        else if ( _operationName == "maxindex" || _operationName == "minindex"){
            _outputRaster->datadefRef().domain(IDomain("count"));
        } else
            _outputRaster->datadefRef().domain(IDomain("value"));
        _outputRaster->size(_inputRaster->size().twod());
        if ( outputName!= sUNDEF)
            _outputRaster->name(outputName);

        OperationHelper::check([&] ()->bool { return _relativeFromRaster.prepare(_expression.input<QString>(2), itRASTER); },
        {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(2), "" } );

        if(!_relativeFromRaster->georeference()->isCompatible(_inputRaster->georeference())){
            ERROR2(ERR_NOT_COMPATIBLE2,"georeference", QString("georeference of ").arg(_inputRaster->name()));
            return sPREPAREFAILED;
        }


        if ( _operationName == "min"){
            _statisticsFunction = stats_min_m;
        } else if ( _operationName == "max"){
            _statisticsFunction = stats_max_m;
        }if ( _operationName == "variance"){
            _statisticsFunction = gsl_stats_variance_m;
        }if ( _operationName == "standarddev"){
            _statisticsFunction = gsl_stats_sd_m  ;
        }if ( _operationName == "totalsumsquares"){
            _statisticsFunction = gsl_stats_tss_m  ;
        }if ( _operationName == "absolutedeviation"){
            _statisticsFunction = gsl_stats_absdev_m  ;
        }if ( _operationName == "autocorrelationlag1"){
            _statisticsFunction = gsl_stats_lag1_autocorrelation_m ;
        }if ( _operationName == "variancefixedmean"){
            _statisticsFunction = gsl_stats_variance_with_fixed_mean  ;
        }if ( _operationName == "standarddevfixedmean"){
            _statisticsFunction = gsl_stats_sd_with_fixed_mean   ;
        }

        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 RelativeAggregateRasterStatistics::createMetadata()
{
    OperationResource operation({"ilwis://operations/relativeaggregaterasterStatistics"},"gsl");
    operation.setSyntax("relativeaggregaterasterStatistics(inputraster,statisticalmarker=variance|standarddev|totalsumsquares|variancefixedmean|standarddevfixedmean|absolutedeviation|max|min|autocorrelationlag1)");
    operation.setDescription(TR("transpose the raster according to the method indicated by the second parameter"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itRASTER,  TR("input raster"),TR("set raster bands to be aggregated"));
    operation.addInParameter(1,itSTRING, TR("statistical method"),TR("method of calucaltion for a pixel column of the stack of bands"));
    operation.addInParameter(2,itRASTER, TR("relative map"),TR("values are calculated relative to the pixel values in this raster"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"), TR("Single band raster with the aggregated statistical values"));
    operation.setKeywords("raster, statistics,numeric");

    mastercatalog()->addItems({operation});
    return operation.id();
}
