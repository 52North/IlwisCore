
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
 * \todo TODO do stretching histogram
 */
bool LinearStretchOperation::stretch(IRasterCoverage toStretch)
{
    NumericStatistics& statistics = _inputRaster->statistics();
    statistics.calculate(begin(_inputRaster), end(_inputRaster), NumericStatistics::pHISTOGRAM);

    // TODO separate histogram into own class (and move
    // certain operations to it
    std::vector<NumericStatistics::HistogramBin> histogram = statistics.histogram();


    SPNumericRange rng = _inputRaster->datadef().range<NumericRange>();
    double valueRange = rng->distance();
    PixelIterator iterInput(_inputRaster);

    std::for_each(begin(_outputRaster), end(_outputRaster), [&](double& v) {
        double vin = *iterInput;
        v = statistics.stretchLinear(vin, valueRange);
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
        ctx->addOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->source() );
    }
    return ok;
}

Ilwis::OperationImplementation::State LinearStretchOperation::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString intputName = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

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
    _outputRaster->setCoordinateSystem(_inputRaster->coordinateSystem());


    _outputRaster->setName(outputName);

    return sPREPARED;
}

/**
 * @todo add input parameter(s) to control stretch range
 * @brief LinearStretchOperation::createMetadata
 * @return the operation's resource id
 */
quint64 LinearStretchOperation::createMetadata()
{
    QString url = QString("ilwis://operations/linearstretch");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","rescale input values to an output map");
    resource.addProperty("syntax","stretch(raster");
    resource.addProperty("description",TR("re-distributes values of an input map over a wider or narrower range of values in an output map. Stretching can for instance be used to enhance the contrast in your map when it is displayed."));
    resource.addProperty("inparameters","1");
    resource.addProperty("pin_1_type", itRASTER);
    resource.addProperty("pin_1_name", TR("rastercoverage to stretch"));
    resource.addProperty("pin_1_desc",TR("input rastercoverage with domain item or numeric"));

    // TODO use intput parameter(s) to control stretch range

    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itRASTER);
    resource.addProperty("pout_1_name", TR("output rastercoverage"));
    resource.addProperty("pout_1_desc",TR("output rastercoverage stretched"));
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}


