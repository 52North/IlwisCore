#include <functional>
#include <future>
#include <QSqlQuery>
#include <QSqlError>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "blockiterator.h"
#include "pixeliterator.h"
#include "rasterfilter.h"
#include "rankorderrasterfilter.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(RankOrderRasterFilter)

RankOrderRasterFilter::RankOrderRasterFilter()
{
}


RankOrderRasterFilter::RankOrderRasterFilter(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool RankOrderRasterFilter::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

   BoxedAsyncFunc filterFun = [&](const BoundingBox& box) -> bool {
        PixelIterator iterOut(_outputRaster, box);
        BlockIterator blockIter(_inputRaster,_filter->size(), box, Size<>(1,1,1));
        PixelIterator iterEnd = iterOut.end();
        while(iterOut != iterEnd) {
            *iterOut = _filter->applyTo(*blockIter);
            ++iterOut;
            ++blockIter;
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

    return false;
}

Ilwis::OperationImplementation *RankOrderRasterFilter::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RankOrderRasterFilter(metaid, expr);
}

Ilwis::OperationImplementation::State RankOrderRasterFilter::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString raster1 = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputRaster.prepare(raster1, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster1,"");
        return sPREPAREFAILED;
    }
    QString name = _expression.parm(1).value();
    int copylist = itDOMAIN | itCOORDSYSTEM | itGEOREF;
    _outputRaster = OperationHelperRaster::initialize(_inputRaster.as<IlwisObject>(),itRASTER, copylist);
    if ( !_outputRaster.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }

    RankOrderGridFilter *filter = new RankOrderGridFilter(name);
    _filter.reset(filter);
    if ( !filter->isValid())
         return sPREPAREFAILED;

    QString query = QString("select rows,columns from filters where code='%1'").arg(name.toLower());
    InternalDatabaseConnection stmt;
    if (stmt.exec(query)) {
        if ( stmt.next()) {
            quint32 rows = stmt.value(0).toInt();
            if ( rows == 0 && _expression.parameterCount() > 3) {// == user-defined
                rows = _expression.parm(2).value().toUInt();
                quint32 columns = _expression.parm(3).value().toUInt();
                filter->colrow(columns,rows);
                if ( _expression.parameterCount() == 5){
                    bool ok;
                    filter->index(_expression.parm(4).value().toUInt(&ok));
                    if (!ok){
                        ERROR2(ERR_ILLEGAL_VALUE_2,TR("parameter"),    _expression.parm(4).value());
                        return sPREPAREFAILED;
                    }
                }
            }
        }
    }
    if ( outputName != sUNDEF)
        _outputRaster->name(outputName);


    return sPREPARED;
}

quint64 RankOrderRasterFilter::createMetadata()
{
    OperationResource operation({"ilwis://operations/rankorderrasterfilter"});
    operation.setSyntax("rankorderrasterfilter(raster1, filtername [,columns,rows,index]");
    operation.setDescription(TR("generates a new raster based on the conditions as specified in the rank order definition"));
    operation.setInParameterCount({2,4,5});
    operation.addInParameter(0,itRASTER , TR("rastercoverage"),TR("input rastercoverage with numeric domain"));
    operation.addInParameter(1,itSTRING , TR("filter name"),TR("Definition of the filter. This can be a predefined filter name"));
    operation.addOptionalInParameter(2,itUINT32 , TR("number of columns"),TR("Number of columns in the user defined matrix, only for user defined rank order filter"));
    operation.addOptionalInParameter(3,itUINT32 , TR("number of rows"),TR("Number of rows in the user defined matrix, only for user defined rank order filter"));
    operation.addOptionalInParameter(4,itUINT32 , TR("index"),TR("Pixel position to be used, only for the rankXxY user defined rank order filter"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("output raster with a numeric domain"));
    operation.setKeywords("filter,raster,image processing");

    mastercatalog()->addItems({operation});
    return operation.id();
}
