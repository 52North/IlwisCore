#ifndef ContainerStatistics_H
#define ContainerStatistics_H

#include "kernel.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/sum.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/kurtosis.hpp>

using namespace boost::accumulators;

namespace Ilwis{

template<typename DataType> class KERNELSHARED_EXPORT ContainerStatistics
{
public:
    typedef accumulator_set<DataType, stats<tag::mean,tag::min, tag::max, tag::count>> Basic;
    typedef accumulator_set<DataType, stats<tag::median>> Median;
    typedef accumulator_set<double, stats<tag::mean, tag::moment<2> > > Variance;
    typedef accumulator_set<double, stats<tag::mean, tag::moment<3> > > Skew;
    typedef accumulator_set<int, stats<tag::kurtosis > > Kurtosis;

    struct HistogramBin{
        HistogramBin(DataType limit=0, quint64 count=0) : _limit(limit), _count(count) {}

        DataType _limit;
        quint64 _count;
    };

    enum PropertySets{pNONE = 0, pBASIC=1, pMIN=2, pMAX=4, pDISTANCE=8, pDELTA=16,pNETTOCOUNT=32, pCOUNT=64, pSUM=128,
                      pMEAN=256, pMEDIAN=512, pPREDOMINANT=1024, pSTDEV=2048, pHISTOGRAM=4096, pVARIANCE=8192, pSKEW=16384,pKURTOSIS=32768,
                      pLAST=65536,pALL=4294967296};
    static const quint32 pNUMERICS = pMIN | pMAX | pDISTANCE | pDELTA | pNETTOCOUNT | pCOUNT | pSUM | pMEAN | pMEDIAN | pPREDOMINANT | pVARIANCE | pSKEW | pKURTOSIS;

    ContainerStatistics(){
        _sigDigits = shUNDEF;
        _markers.resize(index(pLAST));
        DataType undefined = undef<DataType>();
        std::fill(_markers.begin(), _markers.end(), undefined);
    }

    double operator[](PropertySets method) const{
        if ( method == 0)
            return rUNDEF;

        quint32 ind = index(method);
        if ( ind < _markers.size()) {
            if ( hasType(method, pMEAN | pMIN | pMAX | pMEDIAN))
                return (DataType)_markers[ind];

            return _markers[ind]    ;
        }
        return rUNDEF;
    }

    double operator()(PropertySets method, double limit){
        if (_bins.size() == 0)    {
            return rUNDEF;
        }

        limit = _markers[index(pNETTOCOUNT)] * (limit / 100.0);
        double sum = 0;
        if ( method == pMIN){
            for(int i = 0 ; i < _bins.size(); ++i){
                sum += _bins[i]._count;
                if ( sum > limit){
                    return ((double)i / _bins.size()) * ( _markers[index(pMAX)] - _markers[index(pMIN)]) + _markers[index(pMIN)];
                }
            }
        } else if ( method == pMAX){
            double totalSum = _markers[index(pNETTOCOUNT)];
            sum = _markers[index(pNETTOCOUNT)];
            for(int i = _bins.size() - 2 ; i > 0; --i){
                sum -= _bins[i]._count;
                if ( sum < totalSum - limit){
                    return ((double)i / _bins.size()) * ( _markers[index(pMAX)] - _markers[index(pMIN)]) + _markers[index(pMIN)];
                }
            }
        }
        return rUNDEF;

    }

    std::vector<HistogramBin> histogram() {
        return _bins;
    }


    double prop(PropertySets method) const{
        return operator[](method);
    }

    quint16 significantDigits() const {
        return _sigDigits;
    }


    void findSignificantDigits(double distance) {
        if ( distance == 0)
            _sigDigits = 0;
        else{
            double d = prop(pMAX) - prop(pMIN);
            int lenBase = log10(std::abs(d)) / 2;
            QString num = QString::number(distance,'g',10);
            quint32 len = num.size();
            for(int i=num.size() - 2; i >=0; ++i){
                QChar n = num[i];
                if ( n == '.')
                    break;
                if ( n != '0'){
                    len = i;
                    break;
                }
            }
            _sigDigits =  -log10(distance) + std::max( 3 - lenBase,1);
            _sigDigits = std::min(len, _sigDigits);
        }
    }

    void binCount(quint32 value) {
        _binCount = value;
    }



    template<typename IterType> bool calculate(const IterType& begin,  const IterType& end, int mode=pBASIC, int bins = 0, double pseudoUndef=rILLEGAL){
        Basic basicMarkers;
        Median median;
        Variance var;
        Skew skew;
        Kurtosis kurt;
        quint64 count = 0;
        DataType undefined;
        undefined = pseudoUndef != rILLEGAL ? undef<DataType>() : pseudoUndef;
        double sigDigits = 0;
        double rest = 0;
        std::for_each(begin, end, [&] (const DataType& sample){
            count++;
            if ( sample != undefined) {
                rest = fabs(sample - (qint64)sample);
                sigDigits = std::max(sigDigits, rest - sigDigits);
                basicMarkers(sample);
                if ( hasType(mode, pVARIANCE))
                    var(sample);
                if ( hasType(mode, pMEDIAN)) {
                    median(sample);
                }
                if ( hasType(mode, pSKEW)) {
                    skew(sample);
                }
                if ( hasType(mode, pKURTOSIS)) {
                    kurt(sample);
                }

            }});
            bool isUndefined = boost::accumulators::count(basicMarkers) == 0;
            std::fill(_markers.begin(), _markers.end(), rUNDEF);
            if( !isUndefined) {
                _markers[index(pMIN)] = boost::accumulators::min(basicMarkers);
                _markers[index(pMAX)] = boost::accumulators::max(basicMarkers);
                _markers[index(pDISTANCE)] = std::abs(prop(pMAX) - prop(pMIN));
                _markers[index(pDELTA)] = prop(pMAX) - prop(pMIN);
                _markers[index(pNETTOCOUNT)] = boost::accumulators::count(basicMarkers);
                _markers[index(pCOUNT)] = count;
                _markers[index(pSUM)] = boost::accumulators::sum(basicMarkers);
                _markers[index(pMEAN)] = boost::accumulators::mean(basicMarkers);
                _markers[index(pMEDIAN)] = boost::accumulators::median(median);
                _markers[index(pVARIANCE)] = boost::accumulators::moment<2>(var);
                _markers[index(pSKEW)] = boost::accumulators::moment<3>(skew);
                _markers[index(pKURTOSIS)] = boost::accumulators::kurtosis(kurt);
                findSignificantDigits(sigDigits);

                if ( mode & pSTDEV) {
                    _markers[index(pSTDEV)] = calcStdDev(begin, end, undefined);
                }
                if ( mode & pHISTOGRAM) {

                    double ncount = prop(pNETTOCOUNT);
                    if ( ncount > 1) {
                        if (bins == 0 && _binCount == iUNDEF ){
                            if ( prop(pSTDEV) == rUNDEF) {
                                _markers[index(pSTDEV)] = calcStdDev(begin, end, undefined);
                            }
                            if ( _markers[index(pSTDEV)] != rUNDEF ) {
                                double h = 3.5 * _markers[index(pSTDEV)] / pow(ncount, 0.3333);
                                _binCount = prop(pDISTANCE) / h;
                            }
                        } else if(bins != 0){
                            _binCount = bins-1;
                        }
                    }

                    _bins.resize(_binCount + 2); // last cell is for undefines
                    double delta  = prop(pDELTA);
                    for(int i=0; i < _binCount; ++i ) {
                        _bins[i] = HistogramBin(prop(pMIN) + i * ( delta / _binCount));
                    }
                    std::for_each(begin, end, [&] (const DataType& sample){
                        quint16 index = isNumericalUndef(sample) ? _bins.size() - 1 : getOffsetFactorFor(sample);
                        _bins[index]._count++;
                    });
                }
            }

            return true;
        }

        bool isValid() const {
            return prop(pMAX) != rUNDEF;
        }

        double stretchLinear(double input, int stretchRange) const {
            if ( input == rUNDEF)
            return rUNDEF;

            double stretchFactor = stretchRange / prop(pDELTA);
            quint16 index = getOffsetFactorFor(input);
            return _bins[index]._limit * stretchFactor;
        }

        /**
     * Stretches the value range by percent. Lower and upper limits are stretched equally.
     * If stretch value is undefined, the limits of the current value range are returned.
     *
     * @brief stretches lower and upper limits by percent.
     * @param percent how much the limits shall be stretched.
     * @return stretched limits of the value range.
     */
        std::pair<double,double> stretchLimits(double percent) const {
            if (percent == rUNDEF) {
                return std::pair<double,double>(prop(pMIN),prop(pMAX));
            }
            double delta = prop(pDELTA);
            double downsizeBy = percent * delta / 100;
            double newLower = prop(pMIN) + downsizeBy;
            double newUpper = prop(pMAX) - downsizeBy;
            return std::pair<double,double>(newLower,newUpper);
        }

        private:
        std::vector<double> _markers;

                quint32 _sigDigits;
        std::vector<HistogramBin> _bins;
        quint32 _binCount=iUNDEF;

        quint32 index(PropertySets method) const {
            if ( method == 0)
                return 0;
            return (quint32)(log(method) / log(2) + 0.2);
        }

        template<typename IterType> double calcStdDev(const IterType& begin,  const IterType& end, DataType undefined) {
            double ncount = prop(pNETTOCOUNT);
            if ( ncount < 2)
                return rUNDEF;
            double acc = 0;
            double mean = prop(pMEAN);
            std::for_each(begin, end, [&] (const DataType& sample){
                if ( sample != undefined)
                    acc += (sample - mean) * (sample - mean);
            });
            return sqrt(acc / (ncount-1));

        }

        quint16 getOffsetFactorFor(const DataType& sample) const {
            double rmin = prop(pMIN);
            quint16 index = _bins.size() * (double)(sample - rmin) / prop(pDELTA);
            if(index == _bins.size())
                return index - 1;
            else
                return index;
        }

        double getBinWidth() const {
            if (_bins.size() > 1) {
                return _bins[1]._limit;
            } else {
                return _bins[0]._limit;
            }
        }
};
typedef ContainerStatistics<double> NumericStatistics;
}

#endif // ContainerStatistics_H
