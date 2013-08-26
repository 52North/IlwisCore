#ifndef ContainerStatistics_H
#define ContainerStatistics_H

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

using namespace boost::accumulators;

namespace Ilwis{

template<typename DataType> class KERNELSHARED_EXPORT ContainerStatistics
{
public:
    typedef accumulator_set<DataType, stats<tag::mean,tag::min, tag::max, tag::count>> Basic;
    typedef accumulator_set<DataType, stats<tag::median>> Median;

    struct HistogramBin{
        HistogramBin(DataType limit=0, quint64 count=0) : _limit(limit), _count(count) {}

        DataType _limit;
        quint64 _count;
    };

    enum PropertySets{pBASIC=0, pMIN=1, pMAX=2, pDISTANCE=4, pDELTA=8,pNETTOCOUNT=16, pCOUNT=32, pSUM=64,
                      pMEAN=128, pMEDIAN=256, pPREDOMINANT=512, pSTDEV=1024, pHISTOGRAM=2048, pLAST=4096, pALL=4294967296};

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

    template<typename IterType> bool calculate(const IterType& begin,  const IterType& end, PropertySets mode=pBASIC){
        Basic basicMarkers;
        Median median;
        quint64 count = 0;
        DataType undefined = undef<DataType>();
        double sigDigits = 0;
        double rest = 0;
        std::for_each(begin, end, [&] (const DataType& sample){
            count++;
            if ( sample != undefined) {
                 rest = fabs(sample - (qint64)sample);
                 sigDigits = std::max(sigDigits, rest - sigDigits);
                 basicMarkers(sample);
                 if ( hasType(mode, pMEDIAN)) {
                     median(sample);
                 }
            }

        });
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
            findSignificantDigits(sigDigits);

            if ( mode & pSTDEV) {
                _markers[index(pSTDEV)] = calcStdDev(begin, end, undefined);
            }
            if ( mode & pHISTOGRAM) {
                int bins = 1;
                double ncount = prop(pNETTOCOUNT);
                if ( ncount > 1) {
                    if ( prop(pSTDEV) == rUNDEF) {
                        _markers[index(pSTDEV)] = calcStdDev(begin, end, undefined);
                    }
                    if ( _markers[pSTDEV] != rUNDEF ) {
                        double h = 3.5 * _markers[pSTDEV] / pow(ncount, 0.3333);
                        bins = prop(pDISTANCE) / h;
                    }
                }

                _bins.resize(bins);
                double delta  = prop(pDELTA);
                for(int i=0; i < bins; ++i ) {
                    _bins[i] = i * ( delta / bins);
                }
                double rmax = prop(pMAX);
                std::for_each(begin, end, [&] (const DataType& sample){
                    quint16 index = bins * (double)(rmax - sample) / delta;
                    _bins[index++];
                });
            }
        }

        return false;
    }

    bool isValid() const {
        return prop(pMAX) != rUNDEF;
    }

private:
    std::vector<double> _markers;

    quint32 _sigDigits;
    std::vector<HistogramBin> _bins;

    quint32 index(PropertySets method) const{
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



};
typedef ContainerStatistics<double> NumericStatistics;
}

#endif // ContainerStatistics_H
