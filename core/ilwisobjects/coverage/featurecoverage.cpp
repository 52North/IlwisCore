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

FeatureCoverage::FeatureCoverage(const Resource& res) : Coverage(res),_featureTypes(itUNKNOWN),_featureFactory(0)
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

SPFeature &FeatureCoverage::newFeature(const Geometry& geom, quint64 itemId, const SPAttributeRecord& record) {
    _featureTypes |= geom.ilwisType();
    if ( _featureFactory == 0) {
        _featureFactory = kernel()->factory<FeatureFactory>("FeatureFactory","ilwis");
    }

    CreateFeature create = _featureFactory->getCreator("feature");
    Feature *f = create(itemId);
    f->add(geom);
    f->attributeRecord(record);
    SPFeature p(f);
    _features.push_back(p);
    return _features.back();
}



void FeatureCoverage::setFeatureCount(IlwisTypes types, quint32 cnt)
{
    switch(types){
    case itPOINTCOVERAGE:
        _featureInfo[0]._count = cnt;break;
    case itSEGMENTCOVERAGE:
        _featureInfo[1]._count = cnt; break;
    case itPOLYGONCOVERAGE:
        _featureInfo[2]._count = cnt; break;
    }
}

IlwisTypes FeatureCoverage::ilwisType() const
{
    return itFEATURECOVERAGE;
}

quint32 FeatureCoverage::featureCount(IlwisTypes types) const
{
    switch(types){
    case itPOINTCOVERAGE:
        return _featureInfo[0]._count;break;
    case itSEGMENTCOVERAGE:
        return _featureInfo[1]._count; break;
    case itPOLYGONCOVERAGE:
        return _featureInfo[2]._count; break;
    case itFEATURECOVERAGE:
        return _featureInfo[0]._count + _featureInfo[1]._count + _featureInfo[2]._count;

    }
    return 0;
}




