#include "raster.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "mastercatalog.h"
#include "setvaluerange.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(SetValueRange)

SetValueRange::SetValueRange()
{
}

SetValueRange::SetValueRange(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool SetValueRange::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    DataDefinition& datadef = _outputRaster->datadefRef();
    const SPNumericRange numrange = datadef.range<NumericRange>();
    const SPNumericRange rngDomain = datadef.domain()->range<NumericRange>();
    double lmin = _min == rUNDEF ? numrange->min() : std::max(_min, rngDomain->min());
    double lmax = _max == rUNDEF ? numrange->max() : std::min(_max, rngDomain->max());
    double lstep = _step == rUNDEF ? numrange->resolution() : std::max(_step, rngDomain->resolution());
    NumericRange *rng = new NumericRange(lmin, lmax,lstep);
    datadef.range(rng);
    datadef.domain()->range(rng);


    std::function<bool(const BoundingBox)> SetValrange = [&](const BoundingBox box ) -> bool {
        PixelIterator iter(_raster, box);
        PixelIterator iterOut(_outputRaster, box);
        auto end =  iter.end();
        while(iter != end){
            double val = *iter;
            if ( val != rUNDEF){
                double v = rng->ensure(val).value<double>();
                val = v;
            }
            *iterOut = val;
            updateTranquilizer(iterOut.linearPosition(), 1000);
            ++iter;
            ++iterOut;
        };
        return true;
    };
    bool ok = OperationHelperRaster::execute(ctx, SetValrange, _outputRaster);
    _outputRaster->datadefRef().range<NumericRange>()->resolution(_step);
    for(int i=0; i < _outputRaster->size().zsize(); ++i){
        _outputRaster->datadefRef(i).range<NumericRange>()->resolution(_step)    ;
    }

    QVariant value;
    value.setValue<IRasterCoverage>(_outputRaster);
    _outputRaster->addDescription(_expression.toString());
    ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER,_outputRaster->resource() );
    return ok;
}

Ilwis::OperationImplementation *SetValueRange::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SetValueRange(metaid, expr);
}

Ilwis::OperationImplementation::State SetValueRange::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString objectName = _expression.parm(0).value();
    if ( !_raster.prepare(objectName)) {
        ERROR2(ERR_COULD_NOT_LOAD_2, "raster", objectName);
        return sPREPAREFAILED;
    }
    OperationHelperRaster helper;
    _box = helper.initialize(_raster, _outputRaster, itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF|itDOMAIN);

    QString minTxt = _expression.parm(1).value();
    QString maxTxt = _expression.parm(2).value();

    bool ok = true;
    _step = _raster->datadef().domain()->range<NumericRange>()->resolution();
    if ( minTxt.trimmed() != ""){
        _min = minTxt.toDouble(&ok);
    }
    if ( maxTxt.trimmed() != ""){
        _max = maxTxt.toDouble(&ok);
    }
    if ( _expression.parameterCount() == 4) {
        QString stepTxt = _expression.parm(3).value();
        if ( stepTxt.trimmed() != ""){
            _step = stepTxt.toDouble(&ok);
        }
    }
    auto indexes = _raster->stackDefinition().indexes();
    _raster->setDataDefintions(_raster->datadef().domain(),indexes);
    if ( !ok) {
        ERROR2(ERR_ILLEGAL_VALUE_2, TR("parameter"),TR("expression"));
        return sPREPAREFAILED;
    }
    initialize(_outputRaster->size().linearSize());
    return sPREPARED;
}

quint64 SetValueRange::createMetadata()
{

    OperationResource operation({"ilwis://operations/setvaluerange"});
    operation.setSyntax("setvaluerange(objectname,min,max,resolution)");
    operation.setLongName("Set value range raster");
    operation.setDescription(TR("sets the value range of a coverage or column to a new value; all value outside the range will become undefined"));
    operation.setInParameterCount({4});
    operation.addInParameter(0,itRASTER , TR("input object"),TR("input object. If the syntax uses the [] notation it points to a column of a table"));
    operation.addInParameter(1,itNUMBER, TR("minimum"), TR("Color in which the grid lines are drawn, a question mark if this parameter is not used"));
    operation.addInParameter(2,itNUMBER , TR("maximum"), TR("maximum of the new value range. If the value is undefined it will be ignored"));
    operation.addInParameter(3,itNUMBER , TR("resolution"), TR("the minimum distance between to elements of the value range"));
    operation.setOutParameterCount({1});
    operation.addValidation(0,0,"domain=numericdomain");
    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
    operation.setKeywords("numeric,valuerange");

    mastercatalog()->addItems({operation});
    return operation.id();
}
