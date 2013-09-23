#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "gridsize.h"

using namespace Ilwis;
using namespace BaseOperations;

GridSize::GridSize()
{
}

GridSize::GridSize(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool GridSize::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    Size sz = _inputGC->size();
    int dimsize = 0;
    if ( _dim == "xsize")
        dimsize =  sz.xsize();
    else if ( _dim == "ysize")
        dimsize =  sz.ysize();
    else if ( _dim == "zsize")
        dimsize = sz.zsize();
    QVariant var = dimsize;
    ctx->addOutput(symTable,var,sUNDEF,itINT32, Resource());

    return true;
}

OperationImplementation *GridSize::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new GridSize(metaid, expr);
}

OperationImplementation::State GridSize::prepare(ExecutionContext *, const SymbolTable& ){
    QString rasterCoverage = _expression.parm(0).value();
    if (!_inputGC.prepare(rasterCoverage)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,rasterCoverage,"");
        return sPREPAREFAILED;
    }
     QString dim  =_expression.parm(1).value().toLower();
     if ( dim != "xsize" && dim != "ysize" && dim != "zsize") {
         ERROR2(ERR_ILLEGAL_VALUE_2,"dimension value",dim);
         return sPREPAREFAILED;
     }
     _dim = dim;

     return sPREPARED;

}

quint64 GridSize::createMetadata()
{
    QString url = QString("ilwis://operations/gridsize");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","gridsize");
    res.addProperty("syntax","gridsize(gridcoverage,xsize|ysize|zsize)");
    res.addProperty("description",TR("returns the pixel extent of a grid coverage"));
    res.addProperty("inparameters","2");
    res.addProperty("pin_1_type", itRASTER);
    res.addProperty("pin_1_name", TR("gridcoverage"));
    res.addProperty("pin_1_desc",TR("gridcoverage from which a dimension size is retrieved"));
    res.addProperty("pin_2_type", itSTRING);
    res.addProperty("pin_2_name", TR("dimension"));
    res.addProperty("pin_2_desc",TR("Dimension"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itINT32);
    res.addProperty("pout_1_name", TR("size"));
    res.addProperty("pout_1_desc",TR("Lenght in one dimension meausred in grid cells"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}
