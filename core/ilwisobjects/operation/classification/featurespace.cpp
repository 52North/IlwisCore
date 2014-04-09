#include "kernel.h"
#include "location.h"
#include "featurespace.h"

using namespace Ilwis;

FeatureSpace::FeatureSpace()
{
}

void FeatureSpace::addSample(const XY &xy, Raw classRaw)
{
//    if ( classRaw == 255){
//        addSelect(xy);
//    }

//    Raw rawAtXY = 0; //raw(xy);
//    _fsInfo[xy]._fsFrequency++;

//    if (  _fsInfo[xy]._fsFrequency == 1){
//        _fsInfo[xy]._classRaw = classRaw;
//        if ( rawAtXY == raw(xy)){
//            changed(xy);
//        }
//    }

//    Raw originalClass = _fsInfo[xy]._classRaw;
//    Raw newClass = classRaw;



}
