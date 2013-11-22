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

FeatureCoverage::FeatureCoverage() : _featureTypes(itUNKNOWN),_featureFactory(0)
{
    _featureInfo.resize(3);
    _featureInfo[0]._count =  _featureInfo[1]._count =  _featureInfo[2]._count = 0;
}

FeatureCoverage::FeatureCoverage(const Resource& resource) : Coverage(resource),_featureTypes(itUNKNOWN),_featureFactory(0)
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
          Geometry geom = existingFeature->geometry().transform(coordinateSystem(), csySource);
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
    _record.reset(new AttributeRecord(attributeTable(),FEATUREIDCOLUMN ));

    CreateFeature create = _featureFactory->getCreator("feature");
    IFeatureCoverage fcoverage;
    fcoverage.set(this);
    SPFeatureI newFeature(create(this));

    _record.reset(new AttributeRecord(attributeTable(),FEATUREIDCOLUMN ));
    quint32 colIndex = _record->columnIndex(FEATUREIDCOLUMN);
    if ( colIndex == iUNDEF) {
        ERROR1(ERR_NO_INITIALIZED_1, TR("attribute table"));
        return SPFeatureI();
    }
    _record->cellByRecord(_features.size(), colIndex, QVariant(newFeature->featureid()), -1);
    _features.push_back(newFeature);
    quint32 cnt = featureCount(tp);
    setFeatureCount(tp,++cnt );
    return _features.back();
}


void FeatureCoverage::setFeatureCount(IlwisTypes types, quint32 cnt)
{
    Locker lock(_mutex2);
     _featureTypes |= types;
    switch(types){
    case itPOINT:
        _featureInfo[0]._count = cnt;break;
    case itLINE:
        _featureInfo[1]._count = cnt; break;
    case itPOLYGON:
        _featureInfo[2]._count = cnt; break;
    }
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

QSharedPointer<AttributeRecord> FeatureCoverage::record() const
{
    return _record;
}

void FeatureCoverage::copyTo(IlwisObject *obj)
{
    Coverage::copyTo(obj);
    FeatureCoverage *fcov = static_cast<FeatureCoverage *>(obj);
    fcov->_featureTypes = _featureTypes;
    fcov->_featureInfo = _featureInfo;
    fcov->_features = _features;
}

quint32 FeatureCoverage::featureCount(IlwisTypes types, int ) const
{
    switch(types){
    case itPOINT:
        return _featureInfo[0]._count;break;
    case itLINE:
        return _featureInfo[1]._count; break;
    case itPOLYGON:
        return _featureInfo[2]._count; break;
    default:
        return _featureInfo[0]._count + _featureInfo[1]._count + _featureInfo[2]._count;

    }
    return 0;
}

//-----------------------------------------------------------------




