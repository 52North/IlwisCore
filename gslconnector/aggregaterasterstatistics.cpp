#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "operationhelpergrid.h"
#include "geometryhelper.h"
#include "aggregaterasterstatistics.h"
#include "gsl/gsl_statistics.h"

using namespace Ilwis;
using namespace GSL;

REGISTER_OPERATION(AggregateRasterStatistics)

double sumfunc(const double data[], size_t step, size_t count){
    double sum = 0;
    for(int i=0; i < count; i+=step){
        sum += data[i];
    }
    return sum;
}

AggregateRasterStatistics::AggregateRasterStatistics()
{
}

AggregateRasterStatistics::AggregateRasterStatistics(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool AggregateRasterStatistics::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    double inValue;
    bool xchanged = false;
    std::vector<double> zcolumn;
    zcolumn.reserve(_inputRaster->size().zsize());
    PixelIterator iterIn(_inputRaster, BoundingBox(),PixelIterator::fZXY);
    int count = 0;
    for(auto& value : _outputRaster){
        while(!xchanged){
            if(( inValue = *iterIn) != rUNDEF)
                zcolumn.push_back(inValue);
            ++iterIn;
            xchanged = iterIn.xchanged();
        }
        updateTranquilizer(++count, 100);
        if ( _operationName == "median" && zcolumn.size() > 0)
            std::sort(zcolumn.begin(), zcolumn.end());
        value = zcolumn.size() > 0 ? _statisticsFunction1(&zcolumn[0],1,zcolumn.size()) : rUNDEF;
        zcolumn.clear(); // next column
        xchanged = false; // reset flag as we are in the next column now
    }
     _outputRaster->statistics(ContainerStatistics<double>::pBASIC);

     QVariant value;
     value.setValue<IRasterCoverage>(_outputRaster);
    logOperation(_outputRaster,_expression);
     ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource() );

    return true;
}

Ilwis::OperationImplementation *AggregateRasterStatistics::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new AggregateRasterStatistics(metaid,expr);
}

Ilwis::OperationImplementation::State AggregateRasterStatistics::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        OperationHelper::check([&] ()->bool { return _inputRaster.prepare(_expression.input<QString>(0), itRASTER); },
        {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

        std::vector<QString> operations = {"mean","variance","standarddev","totalsumsquares","absolutedeviation",
                                           "skew","kurtosis","max","min","maxindex","minindex","median","autocorrelationlag1","sum"};

        OperationHelper::check([&] ()->bool { return std::find(operations.begin(), operations.end(),_expression.input<QString>(1)) != operations.end(); },
        {ERR_ILLEGAL_VALUE_2,TR("statistical operation"),_expression.input<QString>(1) } );

        _operationName = _expression.input<QString>(1);

         QString outputName = _expression.parm(0,false).value();

       OperationHelperRaster::initialize(_inputRaster, _outputRaster, itCOORDSYSTEM | itGEODETICDATUM | itGEOREF);
       if ( !_outputRaster.isValid()) {
           ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
           return sPREPAREFAILED;
       }
       Envelope env = _inputRaster->envelope().twoD();
       _outputRaster->envelope(env);

       if ( _operationName == "median" || _operationName == "min" || _operationName == "max")
           _outputRaster->datadefRef() = DataDefinition(_inputRaster->datadef().domain(),_inputRaster->datadef().range()->clone());
       else if ( _operationName == "maxindex" || _operationName == "minindex"){
           _outputRaster->datadefRef().domain(IDomain("count"));
       } else
        _outputRaster->datadefRef().domain(IDomain("value"));
       _outputRaster->size(_inputRaster->size().twod());
       if ( outputName!= sUNDEF)
           _outputRaster->name(outputName);

 {
           if ( _operationName == "min"){
               _statisticsFunction1 = gsl_stats_min;
           } else if ( _operationName == "max"){
               _statisticsFunction1 = gsl_stats_max;
           }else if ( _operationName == "median"){
               _statisticsFunction1 = gsl_stats_median_from_sorted_data ;
           }if ( _operationName == "maxindex"){
               _statisticsFunction1 = gsl_stats_max_index ;
           }if ( _operationName == "minindex"){
               _statisticsFunction1 = gsl_stats_min_index  ;
           }if ( _operationName == "variance"){
               _statisticsFunction1 = gsl_stats_variance;
           }if ( _operationName == "standarddev"){
               _statisticsFunction1 = gsl_stats_sd  ;
           }if ( _operationName == "totalsumsquares"){
               _statisticsFunction1 = gsl_stats_tss  ;
           }if ( _operationName == "absolutedeviation"){
               _statisticsFunction1 = gsl_stats_absdev  ;
           }if ( _operationName == "skew"){
               _statisticsFunction1 = gsl_stats_skew   ;
           }if ( _operationName == "kurtosis"){
               _statisticsFunction1 = gsl_stats_kurtosis   ;
           }if ( _operationName == "mean"){
               _statisticsFunction1 = gsl_stats_mean;
           } if ( _operationName == "autocorrelationlag1"){
               _statisticsFunction1 = gsl_stats_lag1_autocorrelation ;
           } if ( _operationName == "sum"){
               _statisticsFunction1 = sumfunc ;
           }
       }
        initialize(_outputRaster->size().linearSize());
        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 AggregateRasterStatistics::createMetadata()
{
    OperationResource operation({"ilwis://operations/aggregaterasterstatistics"},"gsl");
    operation.setSyntax("aggregaterasterstatistics(inputraster,statisticalmarker=mean|variance|standarddev|totalsumsquares|absolutedeviation|skew|kurtosis|max|min|maxindex|minindex|median|sum)");
    operation.setDescription(TR("aggregates the bands of a raster stack into a new rastercoverage according to the method indicated by the second parameter"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER,  TR("input raster"),TR("set raster bands to be aggregated"));
    operation.addInParameter(1,itSTRING, TR("statistical method"),TR("method of calucaltion for a pixel column of the stack of bands"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"), TR("Single band raster with the aggregated statical values"));
    operation.setKeywords("raster, statistics,numeric,aggregate");

    mastercatalog()->addItems({operation});
    return operation.id();
}
