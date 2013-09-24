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

SPFeatureI &FeatureCoverage::newFeature(const Geometry& geom, quint32 itemId, const SPAttributeRecord& record) {
    Locker lock(_mutex);
    _featureTypes |= geom.ilwisType();
    if ( _featureFactory == 0) {
        _featureFactory = kernel()->factory<FeatureFactory>("FeatureFactory","ilwis");
    }

    CreateFeature create = _featureFactory->getCreator("feature");
    FeatureInterface *f = create(itemId);
    f->add(geom);
    f->attributeRecord(record);
    SPFeatureI p(f);
    _features.push_back(p);
    return _features.back();
}

SPFeatureI FeatureCoverage::newFeatureFrom(const SPFeatureI& existingFeature) {
    Locker lock(_mutex);
    if ( existingFeature.isNull() || existingFeature->isValid() == false)
        return SPFeatureI();
    SPFeatureI clonedFeature = existingFeature->clone();
    _features.push_back(clonedFeature);
    quint32 cnt = featureCount(clonedFeature->ilwisType());
    setFeatureCount(clonedFeature->ilwisType(),++cnt );
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

FeatureCoverage *FeatureCoverage::copy()
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




