#include <functional>
#include <future>
#include <algorithm>
#include <cmath>
#include <vector>
#include <utility>
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
#include "aggregationtime.h"
#include "zoneutil.h"
#include "seasonpercentageaggregate.h"

using namespace Ilwis;
using namespace GiaCIS;

REGISTER_OPERATION(SeasonPercentageAggregate)


Ilwis::OperationImplementation *SeasonPercentageAggregate::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SeasonPercentageAggregate(metaid, expr);
}

SeasonPercentageAggregate::SeasonPercentageAggregate()
{
}

double SeasonPercentageAggregate::percentage(std::pair<double, double> lims, double val) {
    int calc = 0;
    if (val > 0) {
        // constrain the value to within the bounds of the <low_perc..high_perc>
        // so the percentage calculated runs from <0..100>
        double pl = lims.first;
        double ph = lims.second;
        val = std::max(pl, std::min(val, ph));
        calc = 100 * (ph - val) / (ph - pl);

        // Add the specific thresholds
        if (calc <= 5) calc = 0;
        else if (calc <= 10) calc = 10;
    }

    return calc;
}

Ilwis::OperationImplementation::State SeasonPercentageAggregate::prepare(ExecutionContext *, const SymbolTable & )
{
    // open the input map list
    QString raster = _expression.parm(0).value();
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

    // Open season table
    QString seasons = _expression.parm(4).value();
    if (!_seasonTable.prepare(seasons, {"mustexist", true})){
        ERROR2(ERR_COULD_NOT_LOAD_2, seasons, "");
        return sPREPAREFAILED;
    }

    // detect if we want a running average
    _doRunning = false;
    if (_expression.parameterCount() == 6)
        _doRunning = _expression.input<bool>(5);

    // setup output map
    _outputObj = OperationHelperRaster::initialize(_inputObj, itRASTER, itGEOREF | itCOORDSYSTEM | itBOUNDINGBOX | itENVELOPE);
    if ( !_outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }

    IRasterCoverage inputRaster = _inputObj.as<RasterCoverage>();
    IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();
    _nb = inputRaster->size().zsize();
    if (_doRunning) {
        auto sz = inputRaster->size();
        outputRaster->size(sz);
    }

    IRasterCoverage zoneRaster = _inputZones.as<RasterCoverage>();
    if ((zoneRaster->size().xsize() != inputRaster->size().xsize()) ||
        (zoneRaster->size().ysize() != inputRaster->size().ysize())) {
        ERROR1(TR("Input maplist and Zone map must have the same spatial dimensions"), "");
        return sPREPAREFAILED;
    }
    // initialize tranquilizer
    initialize(inputRaster->size().xsize() * inputRaster->size().ysize());

    return sPREPARED;
}

bool SeasonPercentageAggregate::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();
    IRasterCoverage inputRaster = _inputObj.as<RasterCoverage>();
    IRasterCoverage zoneRaster = _inputZones.as<RasterCoverage>();

    // Open the input maplist; the first band is assumed to be the first dekad of the year (1st of January)
    // The number of bands may vary, because the data is evaluated every dekad.
    PixelIterator iterIn(inputRaster, BoundingBox(), PixelIterator::fZXY);
    PixelIterator iterZone(zoneRaster, BoundingBox(), PixelIterator::fXYZ); // only one layer so Z is irrelevant
    PixelIterator iterOut(outputRaster, BoundingBox(), PixelIterator::fZXY);

    // read the season table, indicating per zone and dekad if we are in season (start at dekad 1 == 1 january)
    ZoneSeasons<int> zoneSeas;
    zoneSeas.loadFromTable(_seasonTable.as<Table>());

    // read the percentile tables; they are assumed to start at 1st of January
    // also they are assumed to be in 10 day periods (dekads) same as the raster input.
    ZoneLimits<double> limits;
    limits.loadFromTables(_lowPercentile.as<Table>(), _highPercentile.as<Table>());

    IDomain dom;
    dom.prepare("value");
    DataDefinition def(dom);
    def.range(new NumericRange(0, 100, 0)); // we are talking about percentages here
    outputRaster->datadefRef() = def;

    std::vector<double> perc(_nb);
    std::vector<double> psum(_nb);
    auto fromZ = (_doRunning) ? 0 : _nb - 1;
    int incZ = (_doRunning) ? _nb : 1;
    while (iterIn != iterIn.end()) {
        trq()->update(1);

        std::fill(psum.begin(), psum.end(), rUNDEF);

        // get the zone at the current location
        // must be double, to be able to distinguish undef values
        double dzone = *iterZone;
        if (dzone != rUNDEF) {
            int zone = (int) dzone;
            if (zoneSeas.isKnownZone(zone)) {
                ZoneData<int>& zdata = zoneSeas.getSeasonData(zone);

                if ((zdata._startIndex < _nb) && (zdata._length > 0)) {
                    std::fill(psum.begin(), psum.end(), rUNDEF);
                    std::fill(perc.begin(), perc.end(), 0);

                    // get the lower and upper NDVI values for this zone
                    auto lims = limits.getLimits(zone);
                    // only need to consider the values that are in the season
                    // last will be either point to the end of the input or the end of the season (whichever comes first)
                    int last = std::min(zdata._startIndex + zdata._length, _nb);
                    // Calculate the insurance percentage for each of the dekads
                    for (int z = zdata._startIndex; z < last; ++z)
                        perc[z] = percentage(lims[z], *(iterIn + z)) * zdata._data[z];

                    // Accumulate all percentages
                    std::partial_sum(perc.begin() + zdata._startIndex, perc.end(), psum.begin() + zdata._startIndex);
                    // Divide by the length of the season
                    std::transform(psum.begin() + zdata._startIndex, psum.end(), psum.begin() + zdata._startIndex, [&zdata] (const double d) { return d / zdata._length; });

                    // copy the percentage of the last dekad in the season to the following if needed
                    // with single map output: copy last value until end
                    // with running totals: set to undef until end
                    if (last > 0)
                        std::fill(psum.begin() + last, psum.end(), _doRunning ? rUNDEF : psum[last - 1]);
                }
            }
        }
        std::copy(psum.begin() + fromZ, psum.end(), iterOut);
        iterIn += _nb;
        iterOut += incZ;
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

SeasonPercentageAggregate::SeasonPercentageAggregate(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

quint64 SeasonPercentageAggregate::createMetadata()
{
    OperationResource operation({"ilwis://operations/seasonpercentageaggregate"});
    operation.setLongName("Average percentages per season");
    operation.setSyntax("seasonpercentageaggregate(inputgridcoverage,inputzonecoverage,lowpercenttable,highpercenttable,seasontable[,dorunningaverage])");
    operation.setDescription(TR("Determine average payment percentage per growing season and zonal location"));
    operation.setInParameterCount({5,6});
    operation.addInParameter(0, itRASTER, TR("Input rastercoverage"),TR("Input time series rastercoverage with payment percentages"));
    operation.addInParameter(1, itRASTER, TR("Zonal map"),TR("Input map indicating the different zonal areas"));
    operation.addInParameter(2, itTABLE, TR("Low percentile zonal table"),TR("Table contains lower bound NDVI values per decad per zone") );
    operation.addInParameter(3, itTABLE, TR("High percentile zonal table"),TR("Table contains upper bound NDVI values per decad per zone") );
    operation.addInParameter(4, itTABLE, TR("Season table"),TR("Table contains growing season per zonal area and per dekad") );
    operation.addOptionalInParameter(5, itBOOL, TR("Apply running average"),TR("Calculate a running average percentage over the entire maplist") );
    operation.setOutParameterCount({1});
    operation.addOutParameter(0, itRASTER, TR("output rastercoverage"), TR("Average payment percentages"));
    operation.setKeywords("raster, zonal, payment, percentages, aggregate, mean");

    mastercatalog()->addItems({operation});
    return operation.id();
}
