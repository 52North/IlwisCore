#include "kernel.h"
#include "ilwisdata.h"
#include "raster.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "thematicitem.h"
#include "samplestatistics.h"

using namespace Ilwis;

SampleStatistics::SampleStatistics()
{
}

void SampleStatistics::prepare(IThematicDomain thematicDomain, quint32 nrOfBands)
{
    _thematicDomain = thematicDomain;
    _nrOfBands = nrOfBands;

    _sampleSpace.resize(MAXCLASS + 1);
    addClass(MAXCLASS); // == undefined value

    for(auto item : _thematicDomain){
        addClass(item->raw());
    }

}

void SampleStatistics::addClass(Raw key)
{
    if ( _sampleSpace[key].size() != 0)
        return;
    _sampleSpace[key].resize(_nrOfBands);
}

void SampleStatistics::delClass(Raw key)
{
    if ( key >= _sampleSpace.size())
        return;

    _sampleSpace[key].resize(0);
}

void SampleStatistics::resetClass(Raw key)
{
    for(int band = 0; band < _nrOfBands; ++band){
        at(key, band, SampleCell::mMEAN) = 0;
        at(key, band, SampleCell::mSTANDARDDEV) = 0;
    }
}

void SampleStatistics::mergeClass(quint32 key1, quint32 key2, std::unique_ptr<SampleSum> &sumstat, std::unique_ptr<SampleSumXY> &sumXY)
{
    if ( key1 == key2)
        return;
    double sum, mean, num, std;
    for(int band = 0; band < _nrOfBands; ++band){
        num = sumstat->at(key1, _nrOfBands);
        if ( num == 0){
            mean = 0;
            std = 0;
        } else {
            sum = sumstat->at(key1, band);
            mean = sum / num;
            std = std::sqrt((sumXY->at(key1,band,band) - num * mean * mean) / (num - 1.0));
        }
        at(key1, band, SampleCell::mMEAN) = mean;
        at(key1, band, SampleCell::mSTANDARDDEV) = std;
        at(key2, band, SampleCell::mMEAN) = 0;
        at(key2, band, SampleCell::mSTANDARDDEV) = 0;

    }

}

double &SampleStatistics::at(Raw key, quint32 band, SampleCell::Marker marker)
{
    return valueAt(key, band, marker)    ;
}

double SampleStatistics::at(Raw key, quint32 band, SampleCell::Marker marker) const
{
    return const_cast<SampleStatistics *>(this)->valueAt(key, band, marker)    ;
}

double &SampleStatistics::valueAt(Raw key, quint32 band, SampleCell::Marker marker)
{
    if ( key > _thematicDomain->count()){
     throw ErrorObject(TR("SampleStatistics index(es) out of range"));
    }
    if ( marker == SampleCell::mMEAN)
        return _sampleSpace[key][band]._classMean;
    return _sampleSpace[key][band]._classStd;

}

SampleStats *SampleStatistics::clone() const
{
    SampleStatistics  *stats = new SampleStatistics();
    stats->_nrOfBands = _nrOfBands;
    stats->_thematicDomain = _thematicDomain;
    stats->_sampleSpace = _sampleSpace;

    return stats;

}
//----------------------------------------------------------

void SampleSum::prepare(IThematicDomain thematicDomain, quint32 nrOfBands)
{
    _thematicDomain = thematicDomain;
    _nrOfBands = nrOfBands;
    _sums.resize(MAXCLASS + 1);

    addClass(MAXCLASS); // == undefined value

    for(auto item : _thematicDomain){
        addClass(item->raw());
    }
}

void SampleSum::addClass(Raw key)
{
    if ( key >= _sums.size())
        return;

    if ( _sums[key].size() != 0)
        return;

    _sums[key].resize(_nrOfBands + 1,0);
}

void SampleSum::delClass(Raw key)
{
    if ( key < _sums.size())
        _sums[key].resize(0);

}

void SampleSum::resetClass(Raw key)
{
    if ( key >= _sums.size())
        return;
    std::fill(begin(_sums[key]), end(_sums[key]),0);
}

void SampleSum::mergeClass(Raw key1, Raw key2)
{
    if ( key1 == key2)
        return;
    for(int band=0 ; band < _sums[key1].size(); ++band){
        quint32& other = at(key2, band);
        at(key1, band) += other;
        other = 0;
    }
}

double SampleSum::pixelInClass(Raw key)
{
    return (double) at(key, _nrOfBands);
}

SampleStats *SampleSum::clone() const
{
    SampleSum  *sums = new SampleSum();
    sums->_nrOfBands = _nrOfBands;
    sums->_thematicDomain = _thematicDomain;
    sums->_sums = _sums;

    return sums;

}

quint32 &SampleSum::at(Raw raw, quint32 band)
{
    if ( raw >= _sums.size() || band >= _sums[raw].size())
        throw ErrorObject(TR("SampleSum index(es) out of range"));

    return _sums[raw][band];
}

//--------------------------------------------------------------------------
void SampleSumXY::prepare(IThematicDomain thematicDomain, quint32 nrOfBands)
{
    _thematicDomain = thematicDomain;
    _nrOfBands = nrOfBands;
    _sums.resize(MAXCLASS + 1);

    addClass(MAXCLASS); // == undefined value

    for(auto item : _thematicDomain){
        addClass(item->raw());
    }
}

SampleStats *SampleSumXY::clone() const
{
    SampleSumXY  *sums = new SampleSumXY();
    sums->_nrOfBands = _nrOfBands;
    sums->_thematicDomain = _thematicDomain;
    sums->_sums = _sums;

    return sums;

}

void SampleSumXY::addClass(Raw key)
{
    if ( _sums[key].size() != 0)
        return;
    _sums[key].resize(_nrOfBands + 1);
    int count = 0;
    for(auto& vec : _sums[key])
        vec.resize(1 + count++,0);
}

void SampleSumXY::delClass(Raw key)
{
    if ( key >= _sums.size())
        return;

    _sums[key].resize(0);
}

void SampleSumXY::resetClass(Raw key)
{
    for(auto& vec : _sums[key])
        std::fill(vec.begin(),vec.end(), 0);
}

void SampleSumXY::mergeClass(Raw key1, Raw key2)
{
    if ( key1 == key2)
        return;
    for( int band = 0; band < _nrOfBands; ++band){
        for(int index = 0; index <= band; ++index)
            at(key1,band,index) += at(key2, band, index);
    }
    _sums[key2].resize(0);
}

quint32 &SampleSumXY::at(Raw key, quint32 band1, quint32 band2)
{
    if ( key >= _sums.size())
        throw ErrorObject(TR("Sample set index(es) out of bounds"));
    if ( band1 > band2){
        if ( band1 >= _sums[key].size() || band2 >= _sums[key][band1].size())
            throw ErrorObject(TR("Sample set index(es) out of bounds"));
        return _sums[key][band1][band2];
    }
    if ( band2 >= _sums[key].size() || band1 >= _sums[key][band2].size())
        throw ErrorObject(TR("Sample set index(es) out of bounds"));
    return _sums[key][band2][band1];

}

//-----------------------------------------------------------
void SampleHistogram::prepare(IThematicDomain thematicDomain, const IRasterCoverage& raster)
{
    _thematicDomain = thematicDomain;
    _nrOfBands = raster->size().zsize();
    _raster = raster;

    _hist.resize(MAXCLASS + 1);

    addClass(MAXCLASS); // == undefined value

    for(auto item : _thematicDomain){
        addClass(item->raw());
    }
    for(int band = 0; band < _nrOfBands; ++band){
        qint32 minVal = raster->datadef(band).range<NumericRange>()->min();
        if ( isNumericalUndef(minVal))
            minVal = 0;
        _minValuesperBand.push_back(minVal);
    }

}

void SampleHistogram::addClass(Raw key)
{
    if ( _hist[key].size() > 0)
        return;

    _hist[key].resize(_nrOfBands);
    for(int band = 0; band < _nrOfBands; ++band){
        int noOfValues = _raster->datadef(band).range<NumericRange>()->distance();
        _hist[key][band].resize(noOfValues);
        std::fill(_hist[key][band].begin(),_hist[key][band].end(),0);
    }
}

void SampleHistogram::delClass(Raw key)
{
    if ( key >= _hist.size())
        return;
    _hist[key].resize(0);
}

void SampleHistogram::resetClass(Raw key)
{
    if ( key >= _hist.size())
        return;
    if ( _hist[key].size() != _nrOfBands)
        return;

    for(int band = 0; band < _nrOfBands; ++band){
        std::fill(_hist[key][band].begin(),_hist[key][band].end(),0);
    }
}

void SampleHistogram::mergeClass(Raw key1, Raw key2)
{
    for(int band = 0; band < _nrOfBands; ++band){
        int noOfValues = _raster->datadef(band).range<NumericRange>()->distance();
        for( int value=0; value < noOfValues; ++value)
            at(key1,band,value) += at(key2,band, value);
        std::fill(_hist[key2][band].begin(),_hist[key2][band].end(),0);
    }
}

bool SampleHistogram::exists(Raw key) const
{
    if ( key >= _hist.size())
        return false;
    return _hist[key].size() > 0;
}

SampleStats *SampleHistogram::clone() const
{
    SampleHistogram  *hist = new SampleHistogram();
    hist->_nrOfBands = _nrOfBands;
    hist->_raster = _raster;
    hist->_thematicDomain = _thematicDomain;
    hist->_minValuesperBand = _minValuesperBand;
    hist->_hist = _hist;

    return hist;
}

quint32 &SampleHistogram::at(Raw key, quint32 band, quint32 value)
{
    if ( key >= _hist.size())
        throw ErrorObject(TR("Sample set index(es) out of bounds"));
    if ( _hist[key].size() < band)
        throw ErrorObject(TR("Sample set index(es) out of bounds"));
    if ( _hist[key][band].size() < value)
        throw ErrorObject(TR("Sample set index(es) out of bounds"));

    return _hist[key][band][value - _minValuesperBand[band]];
}
