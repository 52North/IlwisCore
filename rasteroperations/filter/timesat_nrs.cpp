#include <functional>
#include <future>
#include <algorithm>
#include <cmath>
#include <vector>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "rasterinterpolator.h"
#include "Eigen/LU"
#include "Eigen/Dense"
#include "timesat_nrs.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(Timesat_nrs)

Ilwis::OperationImplementation *Timesat_nrs::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Timesat_nrs(metaid, expr);
}

Timesat_nrs::Timesat_nrs()
{
}

Ilwis::OperationImplementation::State Timesat_nrs::prepare(ExecutionContext *, const SymbolTable & )
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

    return sPREPARED;
}

Timesat_nrs::Timesat_nrs(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool Timesat_nrs::calcFitWindow(const int i, const int ienvi,
                                const std::vector<double> yfit, const std::vector<bool> wfit,
                                int& m1, int& m2)
{
    m1 = i - _win[ienvi];
    m2 = i + _win[ienvi] + 1;
    auto wm = std::max_element(_win.begin(), _win.end());
    int winmax = *wm;

    //  Adapting fitting interval. Large variation use a smaller window.
    auto xymax = std::minmax_element(yfit.begin() + m1, yfit.begin() + m2);
    double ymin = *xymax.first;
    double ymax = *xymax.second;

    if ( (ymax - ymin) > _win_thresh) {
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
    bool leftFail = itr == wfit.rend();
    m1 = wfit.rend() - itr;
    if (leftFail) ++m1;

    cnt = 0;
    std::vector<bool>::const_iterator it;
    for (it = wfit.begin() + i; (it != wfit.end()) && (cnt < 3); ++it)
         if (*it) cnt++;
    bool rightFail = it == wfit.end();
    m2 = it - wfit.begin();
    if (rightFail) m2 = _nb + 2 * winmax;

    return !(leftFail || rightFail);
}

std::vector<bool> Timesat_nrs::detectSpikes(const std::vector<double> y, std::vector<bool> valid) {
    std::vector<double> y_c;
    copy_if(y.begin(), y.end(), back_inserter(y_c),
            [] (const double d) { return d > 2.0; });

    double N = y_c.size();
    _stats.calculate(y_c.begin(), y_c.end(), NumericStatistics::pSTDEV);
    double ystd = _stats[NumericStatistics::pSTDEV] * std::sqrt((N - 1) / N); // biased stdev
    double distance = _spikecutoff * ystd;

    // Add values around series to handle window effects
    int swinmax = std::floor(_nptperyear / 7); // unsure which value to use: maybe 5 or 7 or 10
    std::vector<double> yext(_nb + swinmax * 2);
    copy(y.begin() + _nb - swinmax, y.end(), yext.begin()); // copy end of series to begin of extension
    copy(y.begin(), y.end(), yext.begin() + swinmax);      // copy series to middle of extension
    copy(y.begin(), y.begin() + swinmax, yext.begin() + _nb + swinmax); // copy begin of series to end of extension

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

std::vector<double> Timesat_nrs::savgol(std::vector<double> y, std::vector<bool> w)
{
    // Adapted code from TIMESAT
    int winmax = *std::max_element(_win.begin(), _win.end());

    // Extend data circularity to improve fitting near the boundary of original data
    std::vector<double> yfit(_nb + winmax * 2);
    std::copy(y.begin() + _nb - winmax, y.end(), yfit.begin()); // copy end of series to begin of extension
    std::copy(y.begin(), y.end(), yfit.begin() + winmax);      // copy series to middle of extension
    std::copy(y.begin(), y.begin() + winmax, yfit.begin() + _nb + winmax); // copy begin of series to end of extension
    std::vector<bool> wfit(_nb + winmax * 2);
    std::copy(w.begin() + _nb - winmax, w.end(), wfit.begin()); // copy end of series to begin of extension
    std::copy(w.begin(), w.end(), wfit.begin() + winmax);      // copy series to middle of extension
    std::copy(w.begin(), w.begin() + winmax, wfit.begin() + _nb + winmax); // copy begin of series to end of extension
    std::vector<int> t(_nb + 2 * winmax);
    int n(-winmax + 1);
    std::generate(t.begin(), t.end(), [&]{ return n++; });

    int nenvi = _win.size();    // number of fitting windows

    double N = yfit.size();
    int m1, m2;
    std::vector<double> dut;
    for (int ienvi = 0; ienvi < nenvi; ++ienvi) {
        _stats.calculate(yfit.begin(), yfit.end(), NumericStatistics::pSTDEV);
        double ystd = _stats[NumericStatistics::pSTDEV] * std::sqrt((N - 1) / N); // biased stdev
        _win_thresh = 1.2 * 2 * ystd; // threshold to adjust the window
        for (int i = winmax; i < _nb + winmax; ++i) {
            if (calcFitWindow(i, ienvi, yfit, wfit, m1, m2)) {
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
            if (_forceUpperEnvelope) {
                int last = _lastIterationLikeTIMESATfit ? nenvi - 1 : nenvi;
                if ( (yfit[i] < y[i - winmax]) && wfit[i] && (ienvi < last))
                    yfit[i] = y[i - winmax];
            }
        }
    }
    std::copy(yfit.begin() + winmax, yfit.begin() + _nb + winmax, y.begin());
    return y;
}


bool Timesat_nrs::execute(ExecutionContext *ctx, SymbolTable& symTable)
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
    int pixPerc = 0;
    int pixStep = rows * cols / 30;
    while (iterIn != inEnd) {
        if (pixCount % pixStep == 0) {
            if (pixPerc % 3 == 0)
                std::cerr << pixPerc * 10 / 3;
            else
                std::cerr << ".";
            pixPerc++;
        }
        pixCount++;

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
        }
        else
            std::fill(fitted.begin(), fitted.end(), 0.0);

        std::copy(fitted.begin(), fitted.end(), iterOut);

        iterIn += _nb;
        iterOut += _nb;
    }
    std::cerr << "100" << std::endl;

    bool resource = true;
    if ( resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(outputRaster);
        ctx->setOutput(symTable, value, outputRaster->name(), itRASTER, outputRaster->source() );
    }
    return resource;
}

quint64 Timesat_nrs::createMetadata()
{
    OperationResource operation({"ilwis://operations/timesat_nrs"});
    operation.setLongName("Timesat filtering");
    operation.setSyntax("timesat_nrs(inputgridcoverage,windowlist,upperenvelop,fitlastiteration");
    operation.setDescription(TR("iteratively filters a rastercoverage with a Savitzky-Golay moving filter"));
    operation.setInParameterCount({4});
    operation.addInParameter(0,itRASTER, TR("Input rastercoverage"),TR("input rastercoverage with value domain"));
    operation.addInParameter(1,itSTRING, TR("Target windowlist"),TR("a list of sizes for the moving window"));
    operation.addInParameter(2,itBOOL, TR("Upper envelop"),TR("Force to original value when fitted value is lower") );
    operation.addInParameter(3,itBOOL, TR("Fit last iteration"),TR("Force upper envelop except last") );
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"), TR("output rastercoverage with the domain of the input map"));
    operation.setKeywords("raster, filter, Savitzky-Golay");

    mastercatalog()->addItems({operation});
    return operation.id();
}
