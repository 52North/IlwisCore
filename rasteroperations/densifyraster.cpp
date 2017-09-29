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
#include "densifyraster.h"


using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(DensifyRaster)


DensifyRaster::DensifyRaster()
{

}

DensifyRaster::DensifyRaster(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr)
{

}

DensifyRaster::~DensifyRaster()
{

}

bool DensifyRaster::isValueMap() { return _inputRaster->datadef().domain<>()->ilwisType() == itNUMERICDOMAIN;}
bool DensifyRaster::isItemMap() { return _inputRaster->datadef().domain<>()->ilwisType() == itITEMDOMAIN;}


bool DensifyRaster::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    // todo: review this operation for item domain rasters
    BoxedAsyncFunc densifyFun = [&](const BoundingBox& box) -> bool {
        PixelIterator iterOut(_outputRaster, box);
        RasterInterpolator interpolator(_inputRaster, _interpolationMethod);
        PixelIterator iterEnd = iterOut.end();
        Pixel inputPosition;
        while(iterOut != iterEnd) {
            inputPosition.x = floor(iterOut.x() / _enlargementFactor);
            inputPosition.y = floor(iterOut.y() / _enlargementFactor);
            inputPosition.z = iterOut.z();
            *iterOut = interpolator.pix2value(inputPosition);
            ++iterOut;
        }
        return true;
    };

    bool resource = OperationHelperRaster::execute(ctx, densifyFun, _outputRaster);

    if ( resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputRaster);
        logOperation(_outputRaster,_expression);
        ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource() );
    }
    return resource;
}

Ilwis::OperationImplementation *DensifyRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new DensifyRaster(metaid, expr);
}


Ilwis::OperationImplementation::State DensifyRaster::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString inputRaster = _expression.parm(0).value();
    QString inputEnlargementFactor = _expression.input<QString>(1);
    QString inputInterpolationMethod = _expression.parm(2).value();
    QString outputRasterName = _expression.parm(0,false).value();

    if (!_inputRaster.prepare(inputRaster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,inputRaster,"");
        return sPREPAREFAILED;
    }

    bool ok;

    _enlargementFactor = inputEnlargementFactor.toDouble(&ok);
    if ( !ok ) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"parameter value","2");
        return sPREPAREFAILED;
    } else {
        if (_enlargementFactor <= 0) {
            ERROR2(ERR_ILLEGAL_VALUE_2,"parameter value","2");
            return sPREPAREFAILED;
        }
    }

    inputInterpolationMethod = _expression.parm(2).value();
    if (inputInterpolationMethod == "nearestneighbour")
        _interpolationMethod = RasterInterpolator::ipNEARESTNEIGHBOUR;
    else if(inputInterpolationMethod == "bilinear")
        _interpolationMethod = RasterInterpolator::ipBILINEAR;
    else if(inputInterpolationMethod == "bicubic")
        _interpolationMethod = RasterInterpolator::ipBICUBIC;
    else {
        ERROR3(ERR_ILLEGAL_PARM_3,"method",inputInterpolationMethod,"mapdensify");
        return sPREPAREFAILED;
    }

    //  For a bilinear or bicubic interpolation, the input raster map should be a value map
    if ((_interpolationMethod == RasterInterpolator::ipBILINEAR  || _interpolationMethod == RasterInterpolator::ipBICUBIC )
            && !isValueMap()) {
        ERROR3(ERR_ILLEGAL_PARM_3,"method",inputInterpolationMethod,"mapdensify");
        return sPREPAREFAILED;
    }

    _outputRaster = OperationHelperRaster::initialize(_inputRaster, itRASTER, itDOMAIN | itCOORDSYSTEM);

    // set georef
    quint32 xsz = (int)(_inputRaster->size().xsize() * _enlargementFactor);
    quint32 ysz = (int)(_inputRaster->size().ysize() * _enlargementFactor);
    QString code= QString("code=georef:type=corners,csy=%1,envelope=%2 %3 %4 %5,gridsize=%6 %7")
            .arg(_inputRaster->coordinateSystem()->resource().url().toString())
            .arg(_inputRaster->envelope().min_corner().x).arg(_inputRaster->envelope().min_corner().y)
            .arg(_inputRaster->envelope().max_corner().x).arg(_inputRaster->envelope().max_corner().y)
            .arg(xsz).arg(ysz);

    IGeoReference georef;
    if(!georef.prepare(code)){
        kernel()->issues()->log(TR("Could not construct georeference based on the given parameters"));
        return sPREPAREFAILED;
    }
    _outputRaster->stackDefinitionRef() = _inputRaster->stackDefinition();
    _outputRaster->georeference(georef);    
    _outputRaster->georeference()->compute();

    Size<> sz = Size <>(_inputRaster->size().xsize() * _enlargementFactor, _inputRaster->size().ysize() * _enlargementFactor, _inputRaster->size().zsize());

    _outputRaster->size(sz);

    _outputRaster->envelope(_inputRaster->envelope());

    if (outputRasterName != sUNDEF){
        _outputRaster->name(outputRasterName);
    }
    ITable inputTbl = _inputRaster->attributeTable();

    if (inputTbl.isValid()) {
        _outputRaster->setAttributes(static_cast<Table *>(inputTbl->clone()));
    }

    return sPREPARED;
}


quint64 DensifyRaster::createMetadata()
{
    OperationResource operation({"ilwis://operations/densifyraster"});
    operation.setSyntax("densifyraster(raster, factor, interpolation=!nearestneighbour | bilinear | bicubic)");
    operation.setDescription(TR("allows you to reduce the pixel size of a map"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itRASTER , TR("input raster"),TR("input rastermap"));
    operation.addInParameter(1,itDOUBLE , TR("enlargement factor"),TR("value by which the number of rows and columns of the input map should be multiplied (real value >= 1)."));
    operation.addInParameter(2,itSTRING , TR("interpolation method"),TR("interpolation method which is used to determine the pixel values in the output map"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("name for the output raster."));
    operation.setKeywords("raster,aggregate,interpolation");

    mastercatalog()->addItems({operation});
    return operation.id();
}
