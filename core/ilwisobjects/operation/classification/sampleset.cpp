#include "kernel.h"
#include "raster.h"
#include "featurespace.h"
#include "thematicitem.h"
#include "itemdomain.h"
#include "samplestatistics.h"
#include "sampleset.h"
#include "ilwisoperation.h"
#include "operationhelpergrid.h"

using namespace Ilwis;

SampleSet::SampleSet()
{
}

void SampleSet::sampleRasterList(const IRasterCoverage &raster)
{
    if ( !hasType(raster->datadef().domain<>()->valueType(), itNUMBER)){
        ERROR2(ERR_INVALID_PROPERTY_FOR_2, TR("Sample list domain"), raster->name());
        return;
    }
    if ( raster->size().zsize() > 1){
        if ( _sampleMap.isValid()){
            if (! _sampleMap->georeference()->isCompatible(raster->georeference())){
                ERROR2(ERR_NOT_COMPATIBLE2,raster->name(), _sampleMap->name());
                return ;
            }
        }
        _sampleMaps = raster;
    }
    else
        ERROR2(ERR_INVALID_PROPERTY_FOR_2, TR("Sample list size"), raster->name());
}

void SampleSet::sampleRaster(const IRasterCoverage& raster){
    if ( _sampleMaps.isValid()){
        if (! _sampleMaps->georeference()->isCompatible(raster->georeference())){
            ERROR2(ERR_NOT_COMPATIBLE2,raster->name(), _sampleMaps->name());
            return ;
        }
    }
    _sampleMap = raster;
}

void SampleSet::thematicDomain(const IThematicDomain &dom)
{
    _sampleDomain = dom;
}

bool SampleSet::prepare()
{
    if (!isValid())
        return false;
    if ( !_sampleMap.isValid()){
        OperationHelperRaster::initialize(_sampleMaps, _sampleMap, itDOMAIN | itGEOREF | itCOORDSYSTEM | itENVELOPE);
    }

    BoundingBox box = _sampleMaps->size();
    for(auto v : _sampleMap){
        PixelIterator iter(_sampleMaps);
        _sampleHistogram(v,iter.position().z, *iter)++;
        _sampleSum(v, iter.position().z) = *iter;
        std::vector<double> shift{0.0,0.0,(double)iter.position().z};
        BoundingBox subBox = box + shift;
        PixelIterator iter2(_sampleMaps, subBox);
        std::for_each(iter2, iter2.end(),[&](double &v) {
            if ( *iter2 == v)
                _sampleSumXY(v,*iter, *iter) += (*iter) * (*iter);
            else
                _sampleSumXY(v, *iter, *iter2) += (*iter2) * (*iter2);

        });

        double sum, mean, num, std;
        for(int band = 0; band < _sampleMaps->size().zsize(); ++band){
            for(auto item : _sampleDomain) {
              num = _sampleSum(item->raw(), _sampleMaps->size().zsize());
              if ( num == 0) {
                mean = std = 0;
              }else {
                 sum = _sampleSum(item->raw(),band);
                 mean = sum / num;
                 if ( num < 1) {
                    std = 0;
                 } else {
                    double v = _sampleSumXY(item->raw(), band, band) - num * mean * mean;
                    std = v * v / ( num - 1);
                 }
              }
              _sampleStats(item->raw(), band, SampleCell::mMEAN)= mean;
              _sampleStats(item->raw(), band, SampleCell::mSTANDARDDEV) = std;

            }
        }
    }

    return true;


}

Raw SampleSet::addClass(const QString& className){
    auto item = _sampleDomain->item(className);
    Raw raw = item->raw();
    if ( !_sampleHistogram.exists(raw)){
        ++_nrOfClasses;
        _sampleHistogram.addClass(raw);
        _sampleStats.addClass(raw);
        _sampleSum.addClass(raw);
        _sampleSumXY.addClass(raw);
    }

    return raw;

}

void SampleSet::deleteClass(Raw raw){
    if ( !_sampleHistogram.exists(raw))
        return;
    _sampleHistogram.delClass(raw);
    _sampleStats.delClass(raw);
    _sampleSum.delClass(raw);
    _sampleSumXY.delClass(raw);

    for(auto& v : _sampleMap)
        if ( v == raw)
            v = iUNDEF;

    for(auto featureSpace : _featureSpaces)
        featureSpace.compute();

}

Raw SampleSet::mergeClass(Raw raw1, Raw raw2){
    if ( raw1 == raw2)
        return raw1;

    _sampleHistogram.mergeClass(raw1, raw2);
    _sampleSum.mergeClass(raw1, raw2);
    _sampleSumXY.mergeClass(raw1, raw2);
    _sampleStats.mergeClass(raw1, raw2, _sampleSum, _sampleSumXY);

    for(auto& v : _sampleMap)
        if ( v == raw2)
            v = raw1;

    for(auto featureSpace : _featureSpaces)
        featureSpace.compute();

    return raw1;

}

void SampleSet::changeSums(const Pixel& pixel, Raw targetClass, int change){
    quint32 nrOfBands = _sampleMaps->size().zsize();
    _sampleSum(targetClass,nrOfBands)+=change;
    for(int band = 0 ; band < nrOfBands; ++band){
        Raw raw = _sampleMaps->pix2value(Pixel(pixel.x, pixel.y, pixel.z));
        _sampleHistogram(targetClass, band, raw)+=change;
        _sampleSum(targetClass, band) += raw;
        for(quint32 band2 = band; band2 < nrOfBands; ++band2){
            if ( band2 == band)
                _sampleSumXY(targetClass,band, band) += change * raw * raw;
            else
                _sampleSumXY(targetClass, band, band2) += change * raw * _sampleMaps->pix2value(Pixel(pixel.x, pixel.y, band2));
        }
    }
}

void SampleSet::decrementSums(const Pixel& pixel, Raw targetClass){
    changeSums(pixel, targetClass, -1);
}

void SampleSet::incrementSums(const Pixel& pixel, Raw targetClass){
    changeSums(pixel, targetClass, 1);
}

void SampleSet::incrementSampels(Raw newClass){
    quint32 nrOfBands = _sampleMaps->size().zsize();
    double mean = 0, std = 0, sum = 0;
    for(int band = 0 ; band < nrOfBands; ++band){

        quint32 num = _sampleSum(newClass, nrOfBands);
        if ( num == 0)
            mean = std = 0;
        else{
            sum =  _sampleSum(newClass, band);
        }
        std = num == 1 ? 0 : std::sqrt(_sampleSumXY(newClass, band, band) - num * mean * mean) / (num - 1);
        _sampleStats(newClass, band, SampleCell::mMEAN) = mean;
    }
}

bool SampleSet::isValid() const
{
    bool ok = _sampleMaps.isValid() && _sampleDomain.isValid();
    ok &= _background.isValid();
    ok &= _sampleMaps->georeference()->isCompatible(_background->georeference());


    return ok;
}

void SampleSet::backgroundRaster(const IRasterCoverage& raster){
    if ( _sampleMaps.isValid()){
        if (! _sampleMaps->georeference()->isCompatible(raster->georeference())){
            ERROR2(ERR_NOT_COMPATIBLE2,raster->name(), _sampleMaps->name());
            return ;
        }
    }
    if ( _sampleMap.isValid()){
        if (! _sampleMap->georeference()->isCompatible(raster->georeference())){
            ERROR2(ERR_NOT_COMPATIBLE2,raster->name(), _sampleMap->name());
            return ;
        }
    }
    _background = raster;
}
