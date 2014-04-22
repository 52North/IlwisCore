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
    PixelIterator iterEnd = iterIn.end();
    while( iterIn != iterEnd)    {
        for(auto item : sampleset().thematicDomain() ){
            bool classFound = true;
            raw = iUNDEF;
            currentClass = _classSequence.at(item->raw());
            while(!iterIn.xchanged()){
                double value = *iterIn;
                quint32 band = iterIn.position().z;
                classFound = classFound &&
                        (value < _boxMax.at(currentClass).at(band) &&
                         value > _boxMin.at(currentClass).at(band));
                ++iterIn;
                if (!classFound)
                    break;
            }
            if ( classFound)  {
                raw = currentClass;
            }
        }
        *iterOut = raw;
        ++iterOut;
    }
    return true;
}

bool BoxClassifier::prepare()
{
    quint32 domainSize = sampleset().thematicDomain()->count();
    std::map<Raw, double> stdProducts;
    for(auto item : sampleset().thematicDomain())
        stdProducts[item->raw()] = std::numeric_limits<double>::max();
    stdProducts[iUNDEF] = std::numeric_limits<double>::max();;

    for(auto item : sampleset().thematicDomain()){
        Raw raw = item->raw();
        for(int band = 0; band < sampleset().sampleRasterSet()->size().zsize(); ++band){
            stdProducts[raw] *= sampleset().statistics()->at(raw,band, SampleCell::mSTANDARDDEV);
            double mean = sampleset().statistics()->at(raw,band, SampleCell::mMEAN);
            if ( mean > 0 && stdProducts[raw] < EPS10){
                return ERROR3(ERR_NO_INITIALIZED_3, "item",sampleset().name(), TR("needs more samples"));
            }
        }
    }
    Raw smallestClass;

    for(auto item1 : sampleset().thematicDomain()) {
        smallestClass = iUNDEF;
        for(auto item2 : sampleset().thematicDomain()){
            if ( stdProducts[item2->raw()] < stdProducts[smallestClass]) {
                smallestClass = item2->raw();
            }
        }
        _classSequence[item1->raw()] = smallestClass;
        stdProducts[smallestClass] = std::numeric_limits<double>::max();
    }

    for(auto item : sampleset().thematicDomain()) {
        for(int band = 0; band < sampleset().sampleRasterSet()->size().zsize(); ++band){
            double std = sampleset().statistics()->at(item->raw(),band, SampleCell::mSTANDARDDEV);
            double mean = sampleset().statistics()->at(item->raw(),band, SampleCell::mMEAN);

            _boxMax[item->raw()].push_back( mean + _widenFactor * std);
            _boxMin[item->raw()].push_back(mean - _widenFactor * std);
        }
    }
    return true;
}
