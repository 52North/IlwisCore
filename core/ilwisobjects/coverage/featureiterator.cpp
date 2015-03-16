#include <iterator>

#include "coverage.h"
#include "geos/geom/Geometry.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"

using namespace Ilwis;

FeatureIterator::FeatureIterator() :
    _isInitial(true)
{
}

FeatureIterator::FeatureIterator(const IFeatureCoverage& fcoverage, quint32 level) : _fcoverage(fcoverage), _isInitial(true),_types(itFEATURE), _level(level)
{
    init();
}

FeatureIterator::FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage, const std::vector<quint32> &subset, quint32 level) :
    _fcoverage(fcoverage),
    _isInitial(true),
    _subset(subset),
    _types(itFEATURE),
    _level(level)
{
    init();
}

FeatureIterator::FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage, IlwisTypes types, quint32 level) :
    _fcoverage(fcoverage),
    _isInitial(true),
    _types(types),
    _level(level)
{

}
FeatureIterator::FeatureIterator(const FeatureIterator &iter)
{
    _fcoverage = iter._fcoverage;
    _isInitial = iter._isInitial;
    _iterFeatures = iter._iterFeatures;
    _subset = iter._subset;
    _iterPosition = iter._iterPosition;
    _types = iter._types;
    _level = iter._level;
    _flow = iter._flow;
}

FeatureIterator &FeatureIterator::operator ++()
{
    if (!init()){
       move();
    }
    return *this;
}


FeatureIterator FeatureIterator::operator ++(int)
{
    init();
    FeatureIterator temp(*this);
    if(!move())
        return end();
    return temp;
}

FeatureIterator &FeatureIterator::operator +(int distance)
{
    if(!init()) {
        move(distance);
    }
    return *this;

}

FeatureIterator &FeatureIterator::operator -(int distance)
{
    if(!init()) {
        move(-distance);
    }
    return *this;
}

bool FeatureIterator::operator ==(const FeatureIterator &iter)
{
    return _iterFeatures == iter._iterFeatures;
}

bool FeatureIterator::operator !=(const FeatureIterator &iter)
{
       return _iterFeatures != iter._iterFeatures;
}

SPFeatureI FeatureIterator::operator *()
{
     init();
     if ( _currentLevel == 0){
         if (_iterFeatures == _fcoverage->_features.end())
             return SPFeatureI();
        return *_iterFeatures;
     }
     else if ( _currentIndexes.size() > 0) {
         quint32 raw = (*_subIterator).first; // index of domain value for subfeature
         QString domvalue = _fcoverage->attributeDefinitionsRef(_currentLevel).index(raw);
         return (*_iterFeatures)[domvalue];
     }

     return SPFeatureI();
}

FeatureIterator FeatureIterator::end() const
{
    FeatureIterator temp(*this);
    temp._iterFeatures = _fcoverage->_features.end();
    return temp;
}

FeatureIterator FeatureIterator::end()
{
    FeatureIterator temp(*this);
    temp._iterFeatures = _fcoverage->_features.end();
    return temp;
}


bool FeatureIterator::init()
{
    if ( _isInitial)     {
        const qint32 rootLevel = 0;
        _currentIndexes = _fcoverage->attributeDefinitions(rootLevel).indexes();
         _currentLevel = _level;
        _useVectorIter = _subset.size() == 0 || _subset.size() == _fcoverage->featureCount();
        _isInitial = false;
        if ( !_fcoverage->connector()->dataIsLoaded()) {
            bool ok = _fcoverage->connector()->loadData(_fcoverage.ptr());
            if (!ok)
                return false;
        }
        if ( _fcoverage->_features.size() > 0 ) {
            _iterPosition = 0;
            _iterFeatures = _fcoverage->_features.begin();
             Feature *feature = static_cast<Feature *>(_fcoverage->_features[rootLevel].get());
            _subIterator = feature->_subFeatures.begin();
            if ( _subset.size() > 0) {
                _iterFeatures += _subset[_iterPosition];
            }
        }
    } else
        return false;
    return true;
}

bool FeatureIterator::moveFlat(qint32 distance){
    if ( _currentLevel != 0)
        return moveBreadthFirst(distance);

    if (_useVectorIter){
        _iterFeatures += distance;
        if ( _iterFeatures == _fcoverage->_features.end()) {
            _iterFeatures = _fcoverage->_features.end();
            return false;
        }
        if (_types != itFEATURE ){ // if we only want to include certain geometry types we need to move until we encounter one
            while (!hasType((*_iterFeatures)->geometryType(),_types))
                move(1);
        }
    }else {
        if ( (qint32)_iterPosition + distance < 0) {
            _iterPosition = 0;
            _iterFeatures = _fcoverage->_features.end();
            return false;
        }
        _iterPosition += distance;
        if ( _iterPosition >= _subset.size()) {
            _iterPosition = _subset.size();
            _iterFeatures = _fcoverage->_features.end();
            return false;
        }
        _iterFeatures = _fcoverage->_features.begin() + _subset[_iterPosition];

    }
    return true;
}

bool FeatureIterator::moveBreadthFirst(qint32 distance){
    // TODO : only one level deep atm, is the most used case if at all. later a full breadth first

    ++_iterFeatures; // move the main feature, as the subiterator is constant this should access level x in the subiterator
    if (_iterFeatures == _fcoverage->_features.end()){ // completed one level, move to next level
        ++_currentLevel;
        _iterFeatures = _fcoverage->_features.begin(); // main feature iterator back to beginning
        ++_subIterator; // next level of subfeatures
    }
    Feature *feature = static_cast<Feature *>((*_iterFeatures).get());
    if ( !feature)
        return false;

    // move fails if there are no more features to iterate on
    return !atEndOfFeatures();
}

bool FeatureIterator::moveDepthFirst(qint32 distance){

    if ( _currentLevel != 0)
        ++_subIterator; // move on subfeatures
    ++_currentLevel;
    Feature *feature = static_cast<Feature *>((*_iterFeatures).get());
    if ( _subIterator == feature->_subFeatures.end()){
        ++_iterFeatures; // to next level 0 feature
        _currentLevel = 0;
        if ( !atEndOfFeatures()) {
            // reset iteration state of subfeatures
            feature = static_cast<Feature *>((*_iterFeatures).get());
           _subIterator = feature->_subFeatures.begin();
        }

    }

    // move fails if there are no more features to iterate on
    return !atEndOfFeatures();

}

bool FeatureIterator::atEndOfFeatures()
{
    return _iterFeatures == _fcoverage->_features.end();
}

bool FeatureIterator::move(qint32 distance) {
    bool ok = false;
    switch ( _flow){
    case fFLAT:
        ok =  moveFlat(distance);
        break;
    case fBREADTHFIRST:
        ok = moveBreadthFirst(distance);
        break;
    case fDEPTHFIRST:
        ok = moveDepthFirst(distance);
        break;
    }
    return ok;
}
FeatureIterator::Flow FeatureIterator::flow() const
{
    return _flow;
}

void FeatureIterator::flow(const Flow &flow)
{
    _flow = flow;
}

