#ifndef ContainerStatistics_H
#define ContainerStatistics_H

#include <vector>
#include <algorithm>
#include <iostream>

namespace Ilwis{

template<typename DataType> class KERNELSHARED_EXPORT ContainerStatistics
{
public:
    struct HistogramBin{
        HistogramBin(DataType limit=0, quint64 count=0) : _limit(limit), _count(count) {}

        DataType _limit;
        quint64 _count;

    };

    enum PropertySets{pBASIC=1, pSTDEV=2, pHISTOGRAM=6, pALL=255};

    ContainerStatistics(){
        _min = _max = undef<DataType>();
        _mean = rUNDEF;
        _stdev = rUNDEF;
        _sum = rUNDEF;
        _count = i64UNDEF;
        _nettoCount = i64UNDEF;
        _sigDigits = shUNDEF;
    }

    double stdev() const{
        return _stdev;
    }
    quint64 count() const{
        return _count;
    }

    quint64 nettoCount() const {
        return _nettoCount;
    }
    DataType max() const{
        return _max;
    }
    DataType min() const{
        return _min;
    }
    double mean() const{
        return _mean;
    }

    quint16 significantDigits() const {
        return _sigDigits;
    }

    void findSignificantDigits(double distance) {
        if ( distance == 0)
            _sigDigits = 0;
        else{
            int lenBase = log10(abs(_max - _min)) / 2;
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
        _count = 0;
        _min = std::numeric_limits<DataType>::max();
        _max = std::numeric_limits<DataType>::min();
        _sum = 0;
        _nettoCount=0;
        DataType undefined = undef<DataType>();
        double sigDigits = 0;
        double rest = 0;
        std::for_each(begin, end, [this,&undefined,&sigDigits,&rest] (const DataType& sample){
           this->_count++;
            if ( sample != undefined) {
                 rest = fabs(sample - (qint64)sample);
                 sigDigits = std::max(sigDigits, rest - sigDigits);
                 this->_nettoCount++;
                 this->_min = std::min( this->_min, sample);
                 this->_max = std::max( this->_max, sample);
                 this->_sum += sample;

            }

        });
        _mean = _sum / _nettoCount;
        findSignificantDigits(sigDigits);

        if ( mode & pSTDEV) {
            double acc = 0;
            std::for_each(begin, end, [&] (const DataType& sample){
                if ( sample != undefined)
                    acc += (sample - _mean) * (sample - _mean);
            });
            _stdev = sqrt(acc / (_nettoCount-1));

            if ( mode & pHISTOGRAM) {
                double h = 3.5 * _stdev / pow(_count, 0.3333);
                int bins = abs(_max - _min) / h;

                _bins.resize(bins);
                double delta  = _max - _min;
                for(int i=0; i < bins; ++i ) {
                    _bins[i] = i * ( delta / bins);
                }

                std::for_each(begin, end, [&] (const DataType& sample){
                    quint16 index = bins * (double)(_max - sample) / delta;
                    _bins[index++];
                });
            }
        }

        return false;
    }

    bool isValid() const {
        return _max != rUNDEF;
    }

private:
    DataType _max;
    DataType _min;
    double _sum;
    double _mean;
    quint64 _count;
    quint64 _nettoCount;
    double _stdev;
    quint32 _sigDigits;
    std::vector<HistogramBin> _bins;



};
typedef ContainerStatistics<double> NumericStatistics;
}

#endif // ContainerStatistics_H
