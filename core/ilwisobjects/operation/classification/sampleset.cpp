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
    if ( !hasType(raster->datadef().domain()->valueType(), itNUMBER)){
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
