#include <functional>
#include <future>
#include <QSqlQuery>
#include <QSqlError>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "stackminmaxfilter.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(StackMinMaxFilter)

StackMinMaxFilter::StackMinMaxFilter()
{
}


StackMinMaxFilter::StackMinMaxFilter(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool StackMinMaxFilter::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

   BoxedAsyncFunc filterFun = [&](const BoundingBox& box) -> bool {
        PixelIterator iterOut(_outputRaster, box, PixelIterator::fZXY);
        PixelIterator iterIn(_inputRaster, box, PixelIterator::fZXY);
        PixelIterator iterEnd = iterIn.end();
        double selectedValue = _filterType == sfMAX ? -1e308 : 1e308;
        while(iterIn != iterEnd) {
            std::vector<double> values(_inputRaster->size().zsize(),0);
            if ( !iterIn.xchanged()){
                double v = *iterIn;
                values[iterIn.z()] = v;
                selectedValue = _filterType == sfMAX ? Ilwis::max(v, selectedValue) : Ilwis::min(v, selectedValue);
            }else {

                for(int i=0; i < values.size(); ++i){
                        *iterOut = values[i] == selectedValue ? selectedValue : 0;
                        ++iterOut;
                }
                values = std::vector<double>(_inputRaster->size().zsize(),0);
            }
            ++iterIn;

            updateTranquilizer(iterIn.linearPosition(), 1000);
        }
        return true;
    };

    bool res = OperationHelperRaster::execute(ctx, filterFun, _outputRaster);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputRaster);
        logOperation(_outputRaster,_expression);
        ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource() );
    }
    return res;

}

Ilwis::OperationImplementation *StackMinMaxFilter::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new StackMinMaxFilter(metaid, expr);
}

Ilwis::OperationImplementation::State StackMinMaxFilter::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
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

    if ( expr.toLower() == "minimum")
        _filterType = sfMIN;
    else if ( expr.toLower() == "maximum")
        _filterType = sfMAX;
    else{
        kernel()->issues()->log(TR("Unknown filter type, only maximum and minimum are allowed, current is: ") + expr);
        return sPREPAREFAILED;
    }

    initialize(_outputRaster->size().linearSize());
    return sPREPARED;
}

quint64 StackMinMaxFilter::createMetadata()
{
    OperationResource operation({"ilwis://operations/stackminmaxpick"});
    operation.setSyntax("stackminmaxpick(raster1, minimum|maximum");
    operation.setDescription(TR("generates a new raster with the same structure as the original but every pixel column \nin the stack will only retain its value if its either the min or the max in his column"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER , TR("rastercoverage"),TR("input rastercoverage with numeric domain"));
    operation.addInParameter(1,itSTRING , TR("filter type"),TR("Definition of the filter type. This can be either minimum or maximum"));
    operation.setOutParameterCount({1});
    operation.addValidation(0,0,"domain=numericdomain");
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("output raster with a numeric domain"));
    operation.setKeywords("filter,raster");

    mastercatalog()->addItems({operation});
    return operation.id();
}
