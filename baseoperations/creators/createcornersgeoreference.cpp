#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "createcornersgeoreference.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateCornersGeoreference)

CreateCornersGeoreference::CreateCornersGeoreference()
{

}

CreateCornersGeoreference::CreateCornersGeoreference(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateCornersGeoreference::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    double xsize = std::ceil((_maxx - _minx) / _pixelsize);
    double ysize = std::ceil((_maxy - _miny) / _pixelsize);
    double deltax = (xsize * _pixelsize - (_maxx - _minx)) / 2;
    double deltay = (ysize * _pixelsize - (_maxy - _miny)) / 2;
    _minx -= deltax;
    _miny -=	deltay;
    _maxx += deltax;
    _maxy += deltay;
    if (!_centerOfPixels)
    {
        xsize += 1;
        ysize += 1;
    }
    QString code= QString("code=georef:type=corners,csy=%1,envelope=%2 %3 %4 %5,gridsize=%6 %7").arg(_csy->resource().url().toString())
            .arg(_minx).arg(_miny).arg(_maxx).arg(_maxy)
            .arg(xsize).arg(ysize);
    if ( _name != ""){
        code += ",name=" + _name;
    }
    IGeoReference georef;
    if(!georef.prepare(code)){
        kernel()->issues()->log(TR("Could not construct georeference based on the given parameters"));
        return false;
    }

    QVariant value;
    value.setValue<IGeoReference>(georef);
    logOperation(georef, _expression);
    ctx->setOutput(symTable,value,georef->name(),itGEOREF,georef->resource());

    return true;
}

Ilwis::OperationImplementation *CreateCornersGeoreference::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateCornersGeoreference(metaid, expr);
}

Ilwis::OperationImplementation::State CreateCornersGeoreference::prepare(ExecutionContext *ctx, const SymbolTable &symbolTable)
{

    bool ok;
    OperationImplementation::prepare(ctx,symbolTable);
    _minx = _expression.input<double>(0,ok);
    if (!ok){
        kernel()->issues()->log(QString(TR("%1 is not a valid minx value").arg(_expression.parm(0).value())));
        return sPREPAREFAILED;
    }
    _miny = _expression.input<double>(1,ok);
    if (!ok){
        kernel()->issues()->log(QString(TR("%1 is not a valid miny value").arg(_expression.parm(1).value())));
        return sPREPAREFAILED;
    }
    _maxx = _expression.input<double>(2,ok);
    if (!ok){
        kernel()->issues()->log(QString(TR("%1 is not a valid maxx value").arg(_expression.parm(2).value())));
        return sPREPAREFAILED;
    }
    _maxy = _expression.input<double>(3,ok);
    if (!ok){
        kernel()->issues()->log(QString(TR("%1 is not a valid maxy value").arg(_expression.parm(3).value())));
        return sPREPAREFAILED;
    }
    if ( (_minx > _maxx) || (_miny > _maxy)){
        kernel()->issues()->log(QString(TR("Envelope of the georef is not correctly defined")));
        return sPREPAREFAILED;
    }
    _pixelsize = _expression.input<double>(4,ok);
    if (!ok){
        kernel()->issues()->log(QString(TR("%1 is not a valid pixel size").arg(_expression.parm(4).value())));
        return sPREPAREFAILED;
    }
    QString csy = _expression.input<QString>(5);
    if (!_csy.prepare(csy)){
        kernel()->issues()->log(QString(TR("%1 is not a valid coordinate system").arg(_expression.parm(5).value())));
        return sPREPAREFAILED;
    }
    _centerOfPixels = _expression.input<bool>(6,ok);
    if (!ok){
        kernel()->issues()->log(QString(TR("%1 is not a boolean value for center of pixels").arg(_expression.parm(6).value())));
        return sPREPAREFAILED;
    }
    _description = _expression.input<QString>(7);
    QString outputName = _expression.parm(0,false).value();
    if ( outputName != sUNDEF)
        _name = outputName;
    return sPREPARED;
}

quint64 CreateCornersGeoreference::createMetadata()
{
    OperationResource resource({"ilwis://operations/createcornersgeoreference"});
    resource.setLongName("Create Corners Georeference");
    resource.setSyntax("createcornersgeoreference(minx, miny, maxx, maxy, pixelsize,coordinatesystem,centerofpixels,description)");
    resource.setInParameterCount({8});
    resource.addInParameter(0, itDOUBLE,TR("Min x"), TR("Minimum X coordinate of the corner of the boundign box"));
    resource.addInParameter(1, itDOUBLE,TR("Min y"), TR("Minimum Y coordinate of the corner of the boundign box"));
    resource.addInParameter(2, itDOUBLE,TR("Max x"), TR("Maximum X coordinate of the corner of the boundign box"));
    resource.addInParameter(3, itDOUBLE,TR("Max y"), TR("Minimum T coordinate of the corner of the boundign box"));
    resource.addInParameter(4, itDOUBLE,TR("Pixel size"), TR("Average size of the pixel"));
    resource.addInParameter(5, itCOORDSYSTEM,TR("Coordinate system"), TR("Coordinate system for which the world coordinates of the georeference are valid"));
    resource.addInParameter(6, itBOOL,TR("Center of Pixels"), TR("Intepretation of how the world coordinate of a pixel should be treated"));
    resource.addInParameter(7, itSTRING,TR("Description"), TR("Extra information about the new georeference"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itGEOREF, TR("output georeference"), TR("The newly created georeference"));
    resource.setKeywords("georeference, create, geometry,workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}


