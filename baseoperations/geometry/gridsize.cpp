#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "gridsize.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(GridSize)

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

    Size<> sz = _inputGC->size();
    int dimsize = 0;
    if ( _dim == "xsize")
        dimsize =  sz.xsize();
    else if ( _dim == "ysize")
        dimsize =  sz.ysize();
    else if ( _dim == "zsize")
        dimsize = sz.zsize();
    QVariant var = dimsize;
    ctx->setOutput(symTable,var,sUNDEF,itINT32, Resource());

    return true;
}

OperationImplementation *GridSize::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new GridSize(metaid, expr);
}

OperationImplementation::State GridSize::prepare(ExecutionContext *ctx, const SymbolTable &st ){
    OperationImplementation::prepare(ctx,st);
    QString raster = _expression.parm(0).value();
    if (!_inputGC.prepare(raster)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
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
    OperationResource operation({"ilwis://operations/rastersize"});
    operation.setSyntax("gridsize(rastercoverage,dimension=xsize|ysize|zsize)");
    operation.setLongName("Raster Gridsize");
    operation.setDescription(TR("returns the pixel extent of a grid coverage dimension"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER, TR("rastercoverage"),TR("rastercoverage from which a dimension size is retrieved"));
    operation.addInParameter(1,itSTRING, TR("dimension"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itINT32, TR("size"),TR("Lenght in one dimension measured in grid cells") );
    operation.setKeywords("raster, size,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();
}
