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
#include "aggregationtime.h"
#include "seasonpercentageaggregate.h"

using namespace Ilwis;
using namespace GiaCIS;

REGISTER_OPERATION(SeasonPercentageAggregate)

template <class T> struct ZoneData
{
    int _zone;
    int _length;
    std::vector<T> _data;

    ZoneData() {}

    bool operator < (const ZoneData& zs) const
    {
        return (_zone < zs._zone);
    }
};

Ilwis::OperationImplementation *SeasonPercentageAggregate::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SeasonPercentageAggregate(metaid, expr);
}

SeasonPercentageAggregate::SeasonPercentageAggregate()
{
}

Ilwis::OperationImplementation::State SeasonPercentageAggregate::prepare(ExecutionContext *, const SymbolTable & )
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

    // Open season table
    QString seasons = _expression.parm(4).value();
    if (!_seasonTable.prepare(seasons, {"mustexist", true})){
        ERROR2(ERR_COULD_NOT_LOAD_2, seasons, "");
        return sPREPAREFAILED;
    }

    // setup output map
    _outputObj = OperationHelperRaster::initialize(_inputObj, itRASTER, itGEOREF | itCOORDSYSTEM | itBOUNDINGBOX | itENVELOPE);
    IRasterCoverage inputRaster = _inputObj.as<RasterCoverage>();
    IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();

    if ( !_outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
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

    ITable low = _lowPercentile.as<Table>();
    ITable high = _highPercentile.as<Table>();

    ITable seasonTable = _seasonTable.as<Table>();

    PixelIterator iterIn(inputRaster, BoundingBox(), PixelIterator::fZXY);
    PixelIterator iterZone(zoneRaster, BoundingBox(), PixelIterator::fXYZ); // only one layer so Z is irrelevant
    PixelIterator iterOut(outputRaster, BoundingBox(), PixelIterator::fZXY);
    PixelIterator inEnd = iterIn.end();

    _nb = inputRaster->size().zsize();
    int rows = inputRaster->size().xsize();
    int cols = inputRaster->size().ysize();
    std::vector<double> slice(_nb);

    // read the percentile tables; they are assumed to start at 1st of January
    int totalRows = low->recordCount(); // same as high->recordCount()
    int totalCols = low->columnCount(); // same as high->columnCount()
    std::vector<ZoneData<double>> lowtab(low->recordCount());
    std::vector<ZoneData<double>> hightab(high->recordCount());
    for (int row = 0; row < totalRows; ++row) {
        lowtab[row]._zone = row + 1;
        hightab[row]._zone = row + 1;
        for (int col = 0; col < totalCols; ++col) {
            lowtab[row]._data.push_back(low->cell(col, row).toDouble());
            hightab[row]._data.push_back(high->cell(col, row).toDouble());
        }
    }

    // Read the seasons table
    int totalSeasonRows = seasonTable->recordCount();
    // The seasons table contains a one for all dekad's that belong to the growing season and
    // zeroes otherwise. The growing season is a consecutive series of ones (no breaks)
    std::vector<ZoneData<int>> seasons(totalSeasonRows);
    for (int row = 0; row < totalSeasonRows; ++row) {
        seasons[row]._zone = seasonTable->cell(0, row).toInt();
        seasons[row]._length = 0;
        for (int col = 1; col < seasonTable->columnCount(); ++col) {
            seasons[row]._data.push_back(seasonTable->cell(col, row).toInt());
        }
        std::vector<int>::iterator sb = seasons[row]._data.begin();
        std::vector<int>::iterator se = seasons[row]._data.end();
        // Count the length of the season: this is at most only one consequtive period
        seasons[row]._length = std::count_if(sb, se, [] (const int d) {return d > 0;});
    }
    std::map<int, int> lutSeason; // add lookup table to get from zone number to season table record
    for (int i = 0; i < seasons.size(); ++i)
        lutSeason[seasons[i]._zone] = i;

    std::map<int, double> lutLowPerc;
    std::map<int, double> lutHighPerc;
    for (int i = 0; i < lowtab.size(); ++i) {
        lutLowPerc[lowtab[i]._zone] = i;
        lutHighPerc[hightab[i]._zone] = i;
    }

    IDomain dom;
    dom.prepare("value");
    DataDefinition def(dom);
    def.range(new NumericRange(0, 100, 0)); // we are talking about percentages here
    outputRaster->datadefRef() = def;

    double accu;
    // timeseries are assumed to be 10 day periods.
    int pixCount = 0;
    while (iterIn != inEnd) {
        trq()->update(pixCount++);

        // get the zone at the current location
        double dzone = *iterZone;
        accu = rUNDEF;
        if (dzone != rUNDEF) {
            int zone = (long) dzone;
            if (lutSeason.find(zone) != lutSeason.end()) {
                int recS = lutSeason[zone];    // point to record with season data for current zone
                int len = seasons[recS]._length;
                if (len > 0) {
                    accu = 0;

                    int recL = lutLowPerc[zone];
                    int recH = lutHighPerc[zone];
                    std::vector<double>::const_iterator plow = lowtab[recL]._data.begin();
                    std::vector<double>::const_iterator phigh = hightab[recH]._data.begin();

                    // accumulate the percentages in the growing season
                    for (int z = 0; z < _nb; ++z) {
                        double ph = *phigh;
                        double pl = *plow;
                        double val = *(iterIn + z);
                        if (val > 0) {
                            val = std::max(pl, std::min(val, ph));
                            double calc = 100.0 * (ph - val) / (ph - pl);
                            if (calc <= 5) calc = 0;
                            else if (calc <= 10) calc = 10;
                            accu += (calc * seasons[recS]._data[z]) / len;
                        }
                        plow++;
                        phigh++;
                    }
                }
            }
        }
        *iterOut = accu;
        iterIn += _nb;
        iterOut += 1;
        iterZone += 1;
    }

    trq()->update(rows * cols);
    trq()->inform("\nWriting...\n");
    trq()->stop();

    bool resource = true;
    if ( resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(outputRaster);
        ctx->setOutput(symTable, value, outputRaster->name(), itRASTER, outputRaster->source() );
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
    operation.setSyntax("seasonpercentageaggregate(inputgridcoverage,inputzonecoverage,seasontable,inputgridstartdate)");
    operation.setDescription(TR("Determine average payment percentage per growing season and zonal location"));
    operation.setInParameterCount({5});
    operation.addInParameter(0,itRASTER, TR("Input rastercoverage"),TR("Input time series rastercoverage with payment percentages"));
    operation.addInParameter(1,itRASTER, TR("Zonal map"),TR("Input map indicating the different zonal areas"));
    operation.addInParameter(2,itTABLE, TR("Low percentile zonal table"),TR("Table contains lower bound NDVI values per decad per zone") );
    operation.addInParameter(3,itTABLE, TR("High percentile zonal table"),TR("Table contains upper bound NDVI values per decad per zone") );
    operation.addInParameter(4,itTABLE, TR("Season table"),TR("Table contains growing season per zonal area and per dekad") );
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"), TR("Average payment percentages"));
    operation.setKeywords("raster, zonal, payment, percentages, aggregate, mean");

    mastercatalog()->addItems({operation});
    return operation.id();
}
