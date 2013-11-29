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
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"

using namespace Ilwis;

FeatureCoverage::FeatureCoverage() : _featureTypes(itUNKNOWN),_featureFactory(0), _maxIndex(0)
{
    _featureInfo.resize(3);
    _featureInfo[0]._count =  _featureInfo[1]._count =  _featureInfo[2]._count = 0;
}

FeatureCoverage::FeatureCoverage(const Resource& resource) : Coverage(resource),_featureTypes(itUNKNOWN),_featureFactory(0), _maxIndex(0)
{
    _featureInfo.resize(3);
    _featureInfo[0]._count =  _featureInfo[1]._count =  _featureInfo[2]._count = 0;
}

IlwisTypes FeatureCoverage::featureTypes() const
{
    return _featureTypes;
}

void FeatureCoverage::featureTypes(IlwisTypes types)
{
    _featureTypes = types;
}

SPFeatureI FeatureCoverage::newFeature(const Geometry& geom) {
    Locker lock(_mutex);
    SPFeatureI newfeature = createNewFeature(geom.ilwisType());
    if (newfeature != nullptr)
        newfeature->set(geom);
    return newfeature;
}

SPFeatureI FeatureCoverage::newFeatureFrom(const SPFeatureI& existingFeature, const ICoordinateSystem& csySource) {
    Locker lock(_mutex);

    SPFeatureI newfeature = createNewFeature(existingFeature->ilwisType());
    if (newfeature == nullptr)
        return newfeature;

    for(int i=0; i < existingFeature->trackSize(); ++i){
          Geometry geom = existingFeature->geometry().transform(csySource);
          newfeature->set(geom, i);
    }
    return newfeature;
}

SPFeatureI FeatureCoverage::createNewFeature(IlwisTypes tp) {
    if ( isReadOnly())
        return SPFeatureI();
    changed(true);

    _featureTypes |= tp;
    if ( _featureFactory == 0) {
        _featureFactory = kernel()->factory<FeatureFactory>("FeatureFactory","ilwis");
    }
    //_record.reset(new AttributeRecord(attributeTable(),FEATUREIDCOLUMN ));

    CreateFeature create = _featureFactory->getCreator("feature");
    IFeatureCoverage fcoverage;
    fcoverage.set(this);
    SPFeatureI newFeature(create(this));
    //fcoverage->attributeTable()->newRecord();

    quint32 colIndex = fcoverage->attributeTable()->columnIndex(FEATUREIDCOLUMN);
    if ( colIndex == iUNDEF) {
        ERROR1(ERR_NO_INITIALIZED_1, TR("attribute table"));
        return SPFeatureI();
    }
    newFeature->setCell(colIndex, QVariant(newFeature->featureid()));
    _features.push_back(newFeature);

    quint32 cnt = featureCount(tp);
    setFeatureCount(tp,++cnt );
    return _features.back();
}


void FeatureCoverage::adaptFeatureCounts(int tp, quint32 cnt, int index) {
    auto adapt = [&] () {
        quint32 current =_featureInfo[tp]._perIndex[index];
        qint32 delta = cnt - _featureInfo[tp]._count;
        _featureInfo[tp]._perIndex[index] = current + delta;
    };

    if ( index < _featureInfo[tp]._perIndex.size()){
        adapt();
    } else {
        if ( index >= _featureInfo[tp]._perIndex.size() ) {
            _featureInfo[tp]._perIndex.resize(index + 1,0);
            _maxIndex = index + 1;
        }
        adapt();
    }
    _featureInfo[tp]._count = cnt;
}

void FeatureCoverage::setFeatureCount(IlwisTypes types, quint32 cnt, int index)
{
    Locker lock(_mutex2);
     _featureTypes |= types;
    switch(types){
    case itPOINT:
        adaptFeatureCounts(0, cnt, index);break;
    case itLINE:
        adaptFeatureCounts(1, cnt, index);break;
    case itPOLYGON:
        adaptFeatureCounts(2, cnt, index);break;
    }
}

quint32 FeatureCoverage::maxIndex() const
{
    return _maxIndex;
}

IlwisTypes FeatureCoverage::ilwisType() const
{
    return itFEATURE;
}

FeatureCoverage *FeatureCoverage::clone()
{
    FeatureCoverage *fcov = new FeatureCoverage();
    copyTo(fcov);
    return fcov;
}

void FeatureCoverage::copyTo(IlwisObject *obj)
{
    Coverage::copyTo(obj);
    FeatureCoverage *fcov = static_cast<FeatureCoverage *>(obj);
    fcov->_featureTypes = _featureTypes;
    fcov->_featureInfo = _featureInfo;
    fcov->_features = _features;
}

quint32 FeatureCoverage::featureCount(IlwisTypes types, int index) const
{
    auto countFeatures = [&] (int tp){
        if ( index == iUNDEF)
            return _featureInfo[tp]._count;
        else
            return  _featureInfo[tp]._perIndex.size() < index ? _featureInfo[tp]._perIndex[index] : 0;
    };

    quint32 count=0;
    if ( hasType(types, itPOINT))
        count += countFeatures(0);
    if ( hasType(types, itLINE))
        count += countFeatures(1);
    if ( hasType(types, itPOLYGON))
        count += countFeatures(2);

    return count;
}

//-----------------------------------------------------------------




