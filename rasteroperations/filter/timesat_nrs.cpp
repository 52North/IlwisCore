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
#include "eigen3/Eigen/LU"
#include "eigen3/Eigen/Dense"
#include "timesat_nrs.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(Timesat)

Ilwis::OperationImplementation *Timesat::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Timesat(metaid, expr);
}

Timesat::Timesat()
{
}

Ilwis::OperationImplementation::State Timesat::prepare(ExecutionContext *, const SymbolTable & )
{
    QString raster = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputObj.prepare(raster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2, raster, "");
        return sPREPAREFAILED;
    }
    _outputObj = OperationHelperRaster::initialize(_inputObj, itRASTER, itDOMAIN | itGEOREF | itCOORDSYSTEM | itRASTERSIZE | itBOUNDINGBOX | itENVELOPE);
    if ( !_outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }

    int winsize = std::max(std::min(_expression.parm(1).value().toInt(), 5), 2);
    _win.resize(winsize);
    int n(1);
    std::generate(_win.begin(), _win.end(), [&]{ return n++; });

    _forceUpperEnvelope = _expression.parm(2).value().endsWith("true", Qt::CaseInsensitive);
    _lastIterationLikeTIMESATfit = _expression.parm(3).value().endsWith("true", Qt::CaseInsensitive);
    _extendWindow = _expression.parm(4).value().endsWith("true", Qt::CaseInsensitive);

    IRasterCoverage inputRaster = _inputObj.as<RasterCoverage>();
    // initialize tranquilizer
    initialize(inputRaster->size().xsize() * inputRaster->size().ysize());

    return sPREPARED;
}

Timesat::Timesat(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool Timesat::calcFitWindow(const int i, const int ienvi,
                                const std::vector<double> yfit, const std::vector<bool> wfit,
                                double win_thresh,
                                int org_offset,
                                int& m1, int& m2)
{
    m1 = i - _win[ienvi];
    m2 = i + _win[ienvi] + 1;

    //  Adapting fitting interval. Large variation use a smaller window.
    auto xymax = std::minmax_element(yfit.begin() + m1, yfit.begin() + m2);
    double ymin = *xymax.first;
    double ymax = *xymax.second;

    if ( (ymax - ymin) > win_thresh) {
        m1 += std::floor(_win[ienvi] / 3); // adjusting the left side with views of m1
        m2 -= std::floor(_win[ienvi] / 3); // adjusting the right side with views of m2
    }

    // Check so that there are enough points, at least 3 at either side
    // with weights different from zero. If not, extend fitting window
    int cnt = 0;
    int offset = wfit.size() - i - 1;
    std::vector<bool>::const_reverse_iterator itr;
    for (itr = wfit.rbegin() + offset; (itr != wfit.rend()) && (cnt < 3); ++itr)
        if (*itr) cnt++;
    int tmp1 = wfit.rend() - itr;
    m1 = std::min(m1, tmp1);
    bool leftFail = cnt < 3;
    if (leftFail) ++m1;

    cnt = 0;
    std::vector<bool>::const_iterator it;
    for (it = wfit.begin() + i; (it != wfit.end()) && (cnt < 3); ++it)
         if (*it) cnt++;
    int tmp2 = it - wfit.begin();
    m2 = std::max(m2, tmp2);
    bool rightFail = cnt < 3;
    if (rightFail) m2 = _nb + 2 * org_offset;

    return !(leftFail || rightFail);
}

std::vector<bool> Timesat::detectSpikes(const std::vector<double> y, std::vector<bool> valid) {
    std::vector<double> y_c;
    copy_if(y.begin(), y.end(), back_inserter(y_c),
            [] (const double d) { return d > 2.0; });

    double N = y_c.size();
    NumericStatistics stats;
    stats.calculate(y_c.begin(), y_c.end(), NumericStatistics::pSTDEV);
    double ystd = stats[NumericStatistics::pSTDEV] * std::sqrt((N - 1) / N); // biased stdev
    double distance = _spikecutoff * ystd;

    int swinmax = 0;
    std::vector<double> yext(_nb);
    if (_extendWindow) {
        // Add values around series to handle window effects
        swinmax = std::floor(_nptperyear / 7); // unsure which value to use: maybe 5 or 7 or 10
        yext.resize(_nb + swinmax * 2);
        copy(y.begin() + _nb - swinmax, y.end(), yext.begin()); // copy end of series to begin of extension
        copy(y.begin(), y.end(), yext.begin() + swinmax);      // copy series to middle of extension
        copy(y.begin(), y.begin() + swinmax, yext.begin() + _nb + swinmax); // copy begin of series to end of extension
    }
    else
        copy(y.begin(), y.end(), yext.begin());

    // find single spikes and set weights to zero
    std::vector<double> dut;
    double med, dev, avg_y, max_y;
    for (int i = swinmax; i < _nb + swinmax; ++i) {
        int m1 = i - swinmax;
        int m2 = i + swinmax + 1;

        dut.clear();
        copy_if(yext.begin() + m1, yext.begin() + m2, back_inserter(dut),
                [] (const double d) { return d > 0.0; });
        if (dut.size() == 0) {
            valid[m1] = false;
            continue;
        }
        std::sort(dut.begin(), dut.end());
        med = dut[dut.size() / 2];

        dev = std::abs(y[m1] - med);
        avg_y = (yext[i - 1] + yext[i + 1]) / 2;
        max_y = std::max(yext[i - 1], yext[i + 1]);
        if (dev >= distance &&
                ( y[m1] < (avg_y - distance) ||
                  y[m1] > (max_y + distance) ) ) valid[m1] = false;
    }

    return valid;
}

std::vector<double> Timesat::savgol(std::vector<double> y, std::vector<bool> w)
{
    // Adapted code from TIMESAT
    int winmax = *std::max_element(_win.begin(), _win.end());

    int nb = _nb;
    std::vector<double> yfit(_nb);
    std::vector<bool> wfit(_nb);
    std::vector<int> t(_nb);
    int org_offset = 0;
    if (_extendWindow) {
        // Extend data circularity to improve fitting near the boundary of original data
        org_offset = winmax;
        yfit.resize(_nb + winmax * 2);
        std::copy(y.begin() + _nb - winmax, y.end(), yfit.begin()); // copy end of series to begin of extension
        std::copy(y.begin(), y.end(), yfit.begin() + winmax);      // copy series to middle of extension
        std::copy(y.begin(), y.begin() + winmax, yfit.begin() + _nb + winmax); // copy begin of series to end of extension
        wfit.resize(_nb + winmax * 2);
        std::copy(w.begin() + _nb - winmax, w.end(), wfit.begin()); // copy end of series to begin of extension
        std::copy(w.begin(), w.end(), wfit.begin() + winmax);      // copy series to middle of extension
        std::copy(w.begin(), w.begin() + winmax, wfit.begin() + _nb + winmax); // copy begin of series to end of extension
        t.resize(_nb + 2 * winmax);
    }
    else {
        nb -= winmax * 2;
        std::copy(y.begin(), y.end(), yfit.begin());
        std::copy(w.begin(), w.end(), wfit.begin());
    }
    int n(-winmax + 1);
    std::generate(t.begin(), t.end(), [&]{ return n++; });

    int nenvi = _win.size();    // number of fitting windows

    double N = yfit.size();
    int m1, m2;
    std::vector<double> dut;
    NumericStatistics stats;
    for (int ienvi = 0; ienvi < nenvi; ++ienvi) {
        stats.calculate(yfit.begin(), yfit.end(), NumericStatistics::pSTDEV);
        double ystd = stats[NumericStatistics::pSTDEV] * std::sqrt((N - 1) / N); // biased stdev
        double win_thresh = 1.2 * 2 * ystd; // threshold to adjust the window
        int last = _lastIterationLikeTIMESATfit ? nenvi - 1 : nenvi;
        for (int i = winmax; i < nb + winmax; ++i) {
            if (calcFitWindow(i, ienvi, yfit, wfit, win_thresh, org_offset, m1, m2)) {
                Eigen::MatrixXd A(3, m2 - m1);
                Eigen::VectorXd b(m2 - m1);
                // preparing data slices as to construct the design matrix
                for (int i = m1; i < m2; ++i) {
                    A(0, i - m1) = wfit[i];
                    A(1, i - m1) = wfit[i] * t[i - m1];
                    A(2, i - m1) = wfit[i] * t[i - m1] * t[i - m1];
                    b(i - m1) = wfit[i] * yfit[i];
                }
                // Solving linear-squares problem A^TAc = A^Tb
                Eigen::Matrix3d ATA = A * A.transpose();
                Eigen::VectorXd ATb = A * b;
                Eigen::VectorXd c = ATA.partialPivLu().solve(ATb);

                // Evaluating the fitted function
                yfit[i] = c(0) + c(1) * t[i - m1] + c(2) * t[i - m1] * t[i - m1];
            }
            else {
                dut.clear();
                copy(yfit.begin() + m1, yfit.begin() + m2, back_inserter(dut));
                std::sort(dut.begin(), dut.end());
                yfit[i] = dut[dut.size() / 2];
            }
            yfit[i] = floor(yfit[i]);   // increase compatibility with IDL version
            if (_forceUpperEnvelope) {
                if ( (yfit[i] < y[i - org_offset]) && wfit[i] && (ienvi < last))
                    yfit[i] = y[i - org_offset];
            }
        }
    }
    std::copy(yfit.begin() + org_offset, yfit.begin() + _nb + org_offset, y.begin());
    return y;
}


bool Timesat::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();
    IRasterCoverage inputRaster = _inputObj.as<RasterCoverage>();

    PixelIterator iterIn(inputRaster, BoundingBox(), PixelIterator::fZXY);
    PixelIterator iterOut(outputRaster, BoundingBox(), PixelIterator::fZXY);
    PixelIterator inEnd = iterIn.end();

    _nb = inputRaster->size().zsize();
    int rows = inputRaster->size().xsize();
    int cols = inputRaster->size().ysize();
    std::vector<double> slice(_nb);
    std::vector<double> fitted(_nb);
    // timeseries are assumed to be 10 day periods.
    int pixCount = 0;
    while (iterIn != inEnd) {
        trq()->update(pixCount++);

        std::copy(iterIn, iterIn + _nb, slice.begin());
        std::vector<bool> valid(_nb);
        std::transform(slice.begin(), slice.end(), valid.begin(),
                       [] (const double d) { return d >= 2.0; });
        int count = std::count(valid.begin(), valid.end(), false);
        if (count < 0.75 * _nb) {
            // determine if the longest period of missing data is greater than 120 days
            auto found = std::search_n(valid.begin(), valid.end(), 12, false);
            if (found == valid.end()) {
                // all missing data period are less than 120 days
                valid = detectSpikes(slice, valid);
                fitted = savgol(slice, valid);
            }
            else
                std::fill(fitted.begin(), fitted.end(), 0.0);
        }
        else
            std::fill(fitted.begin(), fitted.end(), 0.0);

        // make sure output stays in byte range (needed?)
        std::transform(fitted.begin(), fitted.end(), iterOut, [] (const double d) { return std::min(255.0, d); });

        iterIn += _nb;
        iterOut += _nb;
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

quint64 Timesat::createMetadata()
{
    OperationResource operation({"ilwis://operations/timesat"});
    operation.setLongName("Timesat filtering");
    operation.setSyntax("timesat(inputgridcoverage,iterationcount,upperenvelop,fitlastiteration,extendwindow");
    operation.setDescription(TR("iteratively filters a rastercoverage with a Savitzky-Golay moving filter"));
    operation.setInParameterCount({5});
    operation.addInParameter(0,itRASTER, TR("Input rastercoverage"),TR("Input rastercoverage with value domain"));
    operation.addInParameter(1,itSTRING, TR("Iteration count"),TR("Number of iterations with increasing moving window size"));
    operation.addInParameter(2,itBOOL, TR("Upper envelop"),TR("Force to original value when fitted value is lower") );
    operation.addInParameter(3,itBOOL, TR("Fit last iteration"),TR("Force upper envelop except last") );
    operation.addInParameter(4,itBOOL, TR("Extend moving window"),TR("Add values around the data to handle edge values") );
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"), TR("output rastercoverage with the domain of the input map"));
    operation.setKeywords("raster, filter, Savitzky-Golay");

    mastercatalog()->addItems({operation});
    return operation.id();
}
