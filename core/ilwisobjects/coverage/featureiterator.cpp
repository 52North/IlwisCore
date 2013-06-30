#include <iterator>

#include "kernel.h"
#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "polygon.h"
#include "geometry.h"
#include "feature.h"
#include "featurecoverage.h"
#include "featureiterator.h"

using namespace Ilwis;

FeatureIterator::FeatureIterator() :
    _isInitial(true),
    _trackCounter(0),
    _flow(fFEATURES),
    _illegal(FeatureProxy(true))
{
}

FeatureIterator::FeatureIterator(const IFeatureCoverage& fcoverage) : _fcoverage(fcoverage), _isInitial(true), _trackCounter(0),_flow(fFEATURES)
{
    init();
}

FeatureIterator::FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage, const std::vector<quint32> &subset) :
    _fcoverage(fcoverage),
    _isInitial(true),
    _subset(_subset),
    _trackCounter(0),
    _flow(fFEATURES),
    _illegal(FeatureProxy(true))
{

}

FeatureIterator::FeatureIterator(const FeatureIterator &iter)
{
    _fcoverage = iter._fcoverage;
    _isInitial = iter._isInitial;
    _iterFeatures = iter._iterFeatures;
    _trackCounter = iter._trackCounter;
    _flow = iter._flow;
    _subset = iter._subset;
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
    init();
    _iterFeatures = _iterFeatures + distance;
    return *this;

}

FeatureIterator &FeatureIterator::operator -(int distance)
{
    init();
    _iterFeatures = _iterFeatures - distance;
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

FeatureInterface &FeatureIterator::operator *()
{
    init();
    if ( _iterFeatures == _fcoverage->_features.end())
        return _illegal;

    _proxy.setProxy(*_iterFeatures, _trackCounter);
    return _proxy;
}

FeatureIterator FeatureIterator::end() const
{
    FeatureIterator temp(*this);
    temp._iterFeatures = _fcoverage->_features.end();
    return temp;
}

void FeatureIterator::flow(FeatureIterator::Flow f)
{
    _flow = f;
}

bool FeatureIterator::init()
{
    if ( _isInitial)     {
        _isInitial = false;
        bool ok = _fcoverage->connector()->loadBinaryData(_fcoverage.ptr());
        if (!ok)
            return false;
        if ( _fcoverage->_features.size() > 0 ) {
            _iterFeatures = _fcoverage->_features.begin();
            _trackCounter = 0;
        }
    } else
        return false;
    return true;
}

bool FeatureIterator::move() {
    if ( _flow == fFEATURES) {
        ++_iterFeatures;
        if ( _iterFeatures == _fcoverage->_features.end()) {
            _iterFeatures = _fcoverage->_features.begin();
            ++_trackCounter;
        }
        while((*_iterFeatures)->trackSize() <= _trackCounter) {
            ++_iterFeatures;
            if (_iterFeatures == _fcoverage->_features.end())
                return false;
        }
    } else if (_flow == fTRACK) {
        ++_trackCounter;
        if ( _trackCounter >= (*_iterFeatures)->trackSize()) {
            ++_iterFeatures;
            _trackCounter = 0;
            if ( _iterFeatures == _fcoverage->_features.end())
                return false;
        }
    }
    return true;

}
