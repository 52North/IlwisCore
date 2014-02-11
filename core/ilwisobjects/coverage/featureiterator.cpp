#include <iterator>

#include "kernel.h"
#include "coverage.h"
#include "geos/geom/Geometry.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "featureiterator.h"

using namespace Ilwis;

FeatureIterator::FeatureIterator() :
    _isInitial(true)
{
}

FeatureIterator::FeatureIterator(const IFeatureCoverage& fcoverage) : _fcoverage(fcoverage), _isInitial(true),_types(itFEATURE)
{
    init();
}

FeatureIterator::FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage, const std::vector<quint32> &subset) :
    _fcoverage(fcoverage),
    _isInitial(true),
    _subset(subset),
    _types(itFEATURE)
{
    init();
}

FeatureIterator::FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage, IlwisTypes types) :
    _fcoverage(fcoverage),
    _isInitial(true),
    _types(types)
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

UPFeatureI& FeatureIterator::operator *()
{
     init();
     return *_iterFeatures;
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
        _useVectorIter = _subset.size() == 0 || _subset.size() == _fcoverage->featureCount();
        _isInitial = false;
        if ( _fcoverage->_features.size() == 0) {
            bool ok = _fcoverage->connector()->loadBinaryData(_fcoverage.ptr());
            if (!ok)
                return false;
        }
        if ( _fcoverage->_features.size() > 0 ) {
            _iterPosition = 0;
            _iterFeatures = _fcoverage->_features.begin();
            if ( _subset.size() > 0) {
                _iterFeatures += _subset[_iterPosition];
            }
        }
    } else
        return false;
    return true;
}

bool FeatureIterator::move(qint32 distance) {
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
