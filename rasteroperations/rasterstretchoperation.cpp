
#include "kernel.h"
#include "raster.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "numericrange.h"
#include "rasterstrechoperation.h"

using namespace Ilwis;
using namespace RasterOperations;

LinearStretchOperation::LinearStretchOperation()
{
}

LinearStretchOperation::LinearStretchOperation(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

Ilwis::OperationImplementation *LinearStretchOperation::create(quint64 metaid,const Ilwis::OperationExpression& expr)
{
    return new LinearStretchOperation(metaid, expr);
}

/**
 * \todo TODO: do stretching histogram
 */
bool LinearStretchOperation::stretch(IRasterCoverage toStretch)
{
    NumericStatistics& statistics = _inputRaster->statistics();

    // TODO: separate histogram into own class (and move
    // certain operations to it

    SPNumericRange rng = _inputRaster->datadef().range<NumericRange>();
    double valueRange = rng->distance();
    PixelIterator iterInput(_inputRaster);

    std::for_each(begin(_outputRaster), end(_outputRaster), [&](double& v) {
        double vin = *iterInput;
        if(vin >= _limits.first && vin <= _limits.second) {
            v = statistics.stretchLinear(vin, valueRange);
        }
        ++iterInput;
    });

    return true;
}

bool LinearStretchOperation::execute(ExecutionContext *ctx,SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    bool ok = stretch(_inputRaster);

    if ( ok && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputRaster);
        logOperation(_outputRaster,_expression);
        ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource() );
    }
    return ok;
}

Ilwis::OperationImplementation::State LinearStretchOperation::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString intputName = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    int parameterCount = _expression.parameterCount();
    double lower=rUNDEF, upper=rUNDEF;
    lower = _expression.parm(1).value().toDouble();
    if (parameterCount==2 && lower < 0){
        ERROR2(ERR_ILLEGAL_VALUE_2,TR("parameter"),  _expression.parm(0).value());
        return sPREPAREFAILED;
    }
    if (parameterCount == 3) {
        upper = _expression.parm(2).value().toDouble();
        if ( upper < lower) {
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("parameter"),  _expression.parm(0).value());
            return sPREPAREFAILED ;
        }
    }

    if (!_inputRaster.prepare(intputName, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,intputName,"");
        return sPREPAREFAILED;
    }

    _outputRaster = OperationHelperRaster::initialize(_inputRaster, itRASTER, itDOMAIN);
    if ( !_outputRaster.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, outputName);
        return sPREPAREFAILED;
    }
    _outputRaster->georeference(_inputRaster->georeference());
    _outputRaster->coordinateSystem(_inputRaster->coordinateSystem());
    _outputRaster->name(outputName);

    NumericStatistics& statistics = _inputRaster->statistics();
    //statistics.binCount(10);
    statistics.calculate(begin(_inputRaster), end(_inputRaster), NumericStatistics::pHISTOGRAM);

    if (upper == rUNDEF) {
        double percent = lower;
        _limits = statistics.stretchLimits(percent);
    } else
        _limits = std::pair<double, double>(lower, upper);

    _prepState = sPREPARED;
    return _prepState;
}

/**
 * @todo add input parameter(s) to control stretch range
 * @brief LinearStretchOperation::createMetadata
 * @return the operation's resource id
 */
quint64 LinearStretchOperation::createMetadata()
{
    OperationResource operation({"ilwis://operations/linearstretch"});
    operation.setLongName("Linear stretch");
    operation.setSyntax("linearstretch(raster)");
    operation.setDescription(TR("re-distributes values of an input map over a wider or narrower range of values in an output map. Stretching can for instance be used to enhance the contrast in your map when it is displayed."));
    operation.setInParameterCount({2,3});
    operation.addInParameter(0, itRASTER, TR("rastercoverage to stretch"), TR("input rastercoverage with domain item or numeric"));
    operation.addInParameter(1, itNUMERIC, TR("percentage|number"));
    operation.addInParameter(2, itNUMERIC, TR("number"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0, itRASTER, TR("output rastercoverage"), TR("output rastercoverage stretched"));
    operation.setKeywords("raster,image processing,numeric,contrast");

    mastercatalog()->addItems({operation});
    return operation.id();
}


