#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "operationhelpergrid.h"
#include "geometryhelper.h"
#include "rastercovariance.h"
#include "gsl/gsl_statistics.h"

using namespace Ilwis;
using namespace GSL;

REGISTER_OPERATION(RasterCovariance)

RasterCovariance::RasterCovariance()
{
}

RasterCovariance::RasterCovariance(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool RasterCovariance::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    double inValue1, inValue2;
    bool xchanged = false;
    std::vector<double> zcolumn1;
    std::vector<double> zcolumn2;
    zcolumn1.reserve(_inputRaster1->size().zsize());
    zcolumn2.reserve(_inputRaster2->size().zsize());
    PixelIterator iterIn1(_inputRaster1, BoundingBox(),PixelIterator::fZXY);
    PixelIterator iterIn2(_inputRaster2, BoundingBox(),PixelIterator::fZXY);

    for(auto& value : _outputRaster){
        while(!xchanged){
            inValue1 = *iterIn1;
            inValue2 = *iterIn2;
            if(inValue1 != rUNDEF && inValue2 != rUNDEF){
                zcolumn1.push_back(inValue1);
                zcolumn2.push_back(inValue2);
            }
            ++iterIn1;
            ++iterIn2;
            xchanged = iterIn1.xchanged();
        }
        value = zcolumn1.size() > 0 ? gsl_stats_covariance(&zcolumn1[0],1,&zcolumn2[0], 1,zcolumn1.size()) : rUNDEF;
        zcolumn1.clear(); // next column
        zcolumn2.clear();
        xchanged = false; // reset flag as we are in the next column now
    }
    QVariant value;
    value.setValue<IRasterCoverage>(_outputRaster);
    logOperation(_outputRaster,_expression);
    ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource() );
    return true;
}

Ilwis::OperationImplementation *RasterCovariance::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RasterCovariance(metaid,expr);
}

Ilwis::OperationImplementation::State RasterCovariance::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        OperationHelper::check([&] ()->bool { return _inputRaster1.prepare(_expression.input<QString>(0), itRASTER); },
        {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

        OperationHelper::check([&] ()->bool { return _inputRaster2.prepare(_expression.input<QString>(1), itRASTER); },
        {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

        if(!_inputRaster1->georeference()->isCompatible(_inputRaster2->georeference())){
            ERROR2(ERR_NOT_COMPATIBLE2,"georeference", QString("georeference of ").arg(_inputRaster1->name()));
            return sPREPAREFAILED;
        }
        if ( _inputRaster1->size().zsize() != _inputRaster2->size().zsize()){
            ERROR2(ERR_NOT_COMPATIBLE2,"number of bands", _inputRaster1->name());
            return sPREPAREFAILED;
        }

        QString outputName = _expression.parm(0,false).value();

        OperationHelperRaster::initialize(_inputRaster1, _outputRaster, itCOORDSYSTEM | itGEODETICDATUM | itGEOREF);
        if ( !_outputRaster.isValid()) {
            ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
            return sPREPAREFAILED;
        }
        _outputRaster->datadefRef().domain(IDomain("value"));
        _outputRaster->size(_inputRaster1->size().twod());
        if ( outputName!= sUNDEF)
            _outputRaster->name(outputName);



        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 RasterCovariance::createMetadata()
{
    OperationResource operation({"ilwis://operations/covariance"},"gsl");
    operation.setSyntax("covariance(inputraster1,inputraster2)");
    operation.setDescription(TR("calculate the covariance between the pixel value of a stack of rasters (bands); the raster must have equal dimensions"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER,  TR("first raster"),TR("first multi dimensional raster"));
    operation.addInParameter(1,itRASTER, TR("second raster"),TR("second mulit dimensional raster"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"), TR("Single with the covariance between the raster columns"));
    operation.setKeywords("raster, statistics,numeric");

    mastercatalog()->addItems({operation});
    return operation.id();
}
