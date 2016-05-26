#include <functional>
#include <future>
#include <algorithm>
#include <cmath>
#include <vector>
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"
#include "tranquilizerfactory.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "ilwiscontext.h"
#include "rasterinterpolator.h"
#include "basetable.h"
#include "flattable.h"
#include "juliantime.h"
#include "percentilefilterstretch.h"

using namespace Ilwis;
using namespace GiaCIS;

REGISTER_OPERATION(PercentileFilterStretch)

// return dekad number (zero-based)
int dekadFromDate(const Time& date) {
    int dix = (date.get(Time::tpDAYOFMONTH) - 1) / 10;
    int mix = date.get(Time::tpMONTH) - 1;
    return mix * 3 + dix;
}

Ilwis::OperationImplementation *PercentileFilterStretch::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new PercentileFilterStretch(metaid, expr);
}

PercentileFilterStretch::PercentileFilterStretch()
{
}

Ilwis::OperationImplementation::State PercentileFilterStretch::prepare(ExecutionContext *, const SymbolTable & )
{
    QString raster = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    // open the input map list
    if (!_inputObj.prepare(raster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2, raster, "");
        return sPREPAREFAILED;
    }

    // Open the zonal map
    QString zones = _expression.parm(1).value();
    if (!_inputZones.prepare(zones, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2, zones, "");
        return sPREPAREFAILED;
    }

    // Open both percentile tables
    QString lowPerc = _expression.parm(2).value();
    if (!_lowPercentile.prepare(lowPerc, {"mustexist", true})){
        ERROR2(ERR_COULD_NOT_LOAD_2, lowPerc, "");
        return sPREPAREFAILED;
    }

    QString highPerc = _expression.parm(3).value();
    if (!_highPercentile.prepare(highPerc, {"mustexist", true})){
        ERROR2(ERR_COULD_NOT_LOAD_2, highPerc, "");
        return sPREPAREFAILED;
    }

    QString startDate = _expression.parm(4).value();
    Time date(startDate);
    _startDekad = dekadFromDate(date);
    if (!date.isValid()) {
        ERROR1(TR("Invalid date: "), startDate);
        return sPREPAREFAILED;
    }

    _outputObj = OperationHelperRaster::initialize(_inputObj, itRASTER, itDOMAIN | itGEOREF | itCOORDSYSTEM | itRASTERSIZE | itBOUNDINGBOX | itENVELOPE);
    if ( !_outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }

    IRasterCoverage inputRaster = _inputObj.as<RasterCoverage>();
    // initialize tranquilizer
    initialize(inputRaster->size().xsize() * inputRaster->size().ysize());

    return sPREPARED;
}

bool PercentileFilterStretch::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();
    IRasterCoverage inputRaster = _inputObj.as<RasterCoverage>();
    IRasterCoverage zoneRaster = _inputZones.as<RasterCoverage>();

    ITable low = _lowPercentile.as<Table>();
    ITable high = _highPercentile.as<Table>();

    PixelIterator iterIn(inputRaster, BoundingBox(), PixelIterator::fZXY);
    PixelIterator iterZone(zoneRaster, BoundingBox(), PixelIterator::fXYZ); // only one layer so Z is irrelevant
    PixelIterator iterOut(outputRaster, BoundingBox(), PixelIterator::fZXY);
    PixelIterator inEnd = iterIn.end();

    _nb = inputRaster->size().zsize();
    std::vector<double> slice(_nb);
    std::vector<int> percentage(_nb);
    int totalRows = low->recordCount(); // same as high->recordCount()
    int totalCols = low->columnCount(); // same as high->columnCount()
    std::vector<double> lowtab(low->columnCount() * low->recordCount());
    std::vector<double> hightab(high->columnCount() * high->recordCount());
    for (int row = 0; row < totalRows; ++row)
        for (int col = 0; col < totalCols; ++col) {
            // Let the first column in the percentile table match the start of time series
            int actCol = (col + totalCols - _startDekad) % totalCols;
            lowtab[actCol + row * totalCols] = low->cell(col, row).toDouble();
            hightab[actCol + row * totalCols] = high->cell(col, row).toDouble();
        }

    // timeseries are assumed to be 10 day periods.
    while (iterIn != inEnd) {
        trq()->update(1);

        // get the time slice at the current location
        std::copy(iterIn, iterIn + _nb, slice.begin());
        // get the zone at the current location
        double dzone = *iterZone;     // row index into low and high percentile tables
        if (dzone == rUNDEF) {
            // for out of area locations set to zero percent
            std::fill(percentage.begin(), percentage.end(), 0);
        }
        else {
            int zone = (long) dzone;

            std::vector<double>::const_iterator liter = lowtab.begin() + zone * totalCols;
            std::vector<double>::const_iterator hiter = hightab.begin() + zone * totalCols;
            std::vector<int>::iterator piter = percentage.begin();
            for (std::vector<double>::const_iterator siter = slice.begin(); siter != slice.end(); siter++) {
                *piter = std::max(0, std::min(100, int(100.0 * (*hiter - *siter) / (*hiter - *liter))));
                if (*piter <= 5) *piter = 0;
                else if (*piter <= 10) *piter = 10;
                piter++;
                liter++;
                hiter++;
            }
        }
        std::copy(percentage.begin(), percentage.end(), iterOut);
        iterIn += _nb;
        iterOut += _nb;
        iterZone += 1;
    }

    trq()->inform("\nWriting...\n");
    trq()->stop();

    bool resource = true;
    if ( resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(outputRaster);
        ctx->setOutput(symTable, value, outputRaster->name(), itRASTER, outputRaster->resource() );
    }
    return resource;
}

PercentileFilterStretch::PercentileFilterStretch(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

quint64 PercentileFilterStretch::createMetadata()
{
    OperationResource operation({"ilwis://operations/percentilefilterstretch"});
    operation.setLongName("NDVI percentile stretching");
    operation.setSyntax("percentilefilterstretch(inputgridcoverage,inputzonecoverage,lowpercentiletable,highpercentiletable,inputgridstartdate)");
    operation.setDescription(TR("Determine payment percentage based on time of year and zonal location"));
    operation.setInParameterCount({5});
    operation.addInParameter(0,itRASTER, TR("Input rastercoverage"),TR("Input time series rastercoverage with NDVI values"));
    operation.addInParameter(1,itRASTER, TR("Zonal map"),TR("Input map indicating the different zonal areas"));
    operation.addInParameter(2,itTABLE, TR("Low percentile zonal table"),TR("Table contains lower bound NDVI values per decad per zone") );
    operation.addInParameter(3,itTABLE, TR("High percentile zonal table"),TR("Table contains upper bound NDVI values per decad per zone") );
    operation.addInParameter(4,itSTRING, TR("NDVI coverage start date"),TR("Date at the start of the first dekad in the input NDVI coverage") );
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"), TR("Payment percentages"));
    operation.setKeywords("raster, zonal, payment, percentages");

    mastercatalog()->addItems({operation});
    return operation.id();
}
