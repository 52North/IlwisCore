#include <unordered_map>
#include "kernel.h"
#include "ilwisdata.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "location.h"
#include "raster.h"
#include "pixeliterator.h"
#include "classification/samplestatistics.h"
#include "classification/sampleset.h"
#include "classifier.h"

using namespace Ilwis;
using namespace RasterOperations;

Classifier::Classifier(const SampleSet &sampleset) : _sampleset(sampleset)
{
}

//-------------------------------------------------------------------
BoxClassifier::BoxClassifier(double widenFactor, const SampleSet &sampleset) : Classifier(sampleset), _widenFactor(widenFactor){

}

bool BoxClassifier::classify(PixelIterator &iterIn, PixelIterator &iterOut) const
{
    Raw currentClass, raw;
    PixelIterator iterEnd = iterOut.end();
    std::vector<double> zcolumn(sampleset().sampleRasterSet()->size().zsize(), rUNDEF);
    while( iterOut != iterEnd)    {
        for(auto item : sampleset().thematicDomain() ){
            raw = iUNDEF;
            currentClass = _classSequence.at(item->raw());
            bool classFound = true;

            for(double& v : zcolumn){
                v = *iterIn;
                ++iterIn;
            }
            for(int band = 0; band < zcolumn.size(); ++band){
                classFound = classFound &&
                        (zcolumn[band] < _boxMax.at((quint32)currentClass).at(band) &&
                         zcolumn[band] > _boxMin.at((quint32)currentClass).at(band));
                if (!classFound)
                    break;
            }
            if ( classFound)  {
                raw = currentClass;
                break;
            }
        }
        *iterOut = raw;
        ++iterOut;
    }
    return true;
}

bool BoxClassifier::prepare()
{
    std::map<Raw, double> stdProducts;
    for(auto item : sampleset().thematicDomain())
        stdProducts[item->raw()] = 1; //std::numeric_limits<double>::max();
    stdProducts[iUNDEF] = 1; //std::numeric_limits<double>::max();;


    Raw maxRaw = rUNDEF;
    for(auto item : sampleset().thematicDomain()){
        Raw raw = item->raw();
        maxRaw = Ilwis::max(raw, maxRaw);
        for(int band = 0; band < sampleset().sampleRasterSet()->size().zsize(); ++band){
            stdProducts[raw] *= sampleset().statistics()->at(raw,band, SampleCell::mSTANDARDDEV);
            double mean = sampleset().statistics()->at(raw,band, SampleCell::mMEAN);
            if ( mean > 0 && stdProducts[raw] < EPS10){
                return ERROR3(ERR_NO_INITIALIZED_3, "item",sampleset().name(), TR("needs more samples"));
            }
        }
    }
    _classSequence.resize(maxRaw + 1);
    _boxMax.resize(maxRaw + 1);
    _boxMin.resize(maxRaw + 1);
    Raw smallestClass;
    stdProducts[iUNDEF] = std::numeric_limits<double>::max();
    for(auto item1 : sampleset().thematicDomain()) {
        smallestClass = iUNDEF;
        for(auto item2 : sampleset().thematicDomain()){
            if ( stdProducts[item2->raw()] < stdProducts[smallestClass]) {
                smallestClass = item2->raw();
            }
        }
        int raw = item1->raw();
        _classSequence[(quint32)raw] = smallestClass;
        stdProducts[smallestClass] = std::numeric_limits<double>::max();
    }
    for(auto item : sampleset().thematicDomain()) {
        for(int band = 0; band < sampleset().sampleRasterSet()->size().zsize(); ++band){
            double std = sampleset().statistics()->at(item->raw(),band, SampleCell::mSTANDARDDEV);
            double mean = sampleset().statistics()->at(item->raw(),band, SampleCell::mMEAN);

            _boxMax[(quint32)item->raw()].push_back( mean + _widenFactor * std);
            _boxMin[(quint32)item->raw()].push_back(mean - _widenFactor * std);
        }
    }
    return true;
}
