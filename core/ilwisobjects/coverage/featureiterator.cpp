#include <iterator>
#include <QString>
#include <QVector>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include "kernel.h"
#include "ilwis.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "line.h"
#include "geometry.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "coordinatesystem.h"
#include "valuedefiner.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"
#include "containerstatistics.h"
#include "coverage.h"
#include "connectorinterface.h"
#include "resource.h"
#include "featurecoverage.h"
#include "featureiterator.h"

using namespace Ilwis;

FeatureIterator::FeatureIterator(const IFeatureCoverage& fcoverage) : _fcoverage(fcoverage), _isInitial(true), _trackCounter(0),_flow(fFEATURES)
{
}

FeatureIterator::FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage, const Box3D<double>& envelope) :
    _fcoverage(fcoverage),
    _isInitial(true),
    _envelope(envelope),
    _trackCounter(0),
    _flow(fFEATURES)
{

}

FeatureIterator::FeatureIterator(const FeatureIterator &iter)
{
    _fcoverage = iter._fcoverage;
    _isInitial = iter._isInitial;
    _iterFeatures = iter._iterFeatures;
    _trackCounter = iter._trackCounter;
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
        _fcoverage->connector()->loadBinaryData(_fcoverage.ptr());
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
        while((*_iterFeatures)->_track.size() < _trackCounter) {
            ++_iterFeatures;
            if (_iterFeatures == _fcoverage->_features.end())
                return false;
        }
    } else if (_flow == fTRACK) {
        ++_trackCounter;
        if ( _trackCounter >= (*_iterFeatures)->_track.size()) {
            ++_iterFeatures;
            _trackCounter = 0;
            if ( _iterFeatures == _fcoverage->_features.end())
                return false;
        }
    }
    return true;

}
