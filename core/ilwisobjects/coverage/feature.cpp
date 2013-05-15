#include <QStringList>
#include <QSharedPointer>
#include "kernel.h"
#include "ilwis.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "line.h"
#include "geometry.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "valuedefiner.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"

using namespace Ilwis;

quint64 Feature::_idbase = 0;

QVariant FeatureInterface::operator [](const QString &name) {
    return value(name);
}

//--------------------------------------------
Feature::Feature() : _itemid(i64UNDEF), _featureid(i64UNDEF){
}

Feature::~Feature()
{
}
Feature::Feature(const Feature &f) {
}

Feature &Feature::operator =(const Feature &f)
{
    return *this;
}

QVariant Feature::value(const QString &name)
{
    return _record->cellByKey(itemId(), name);
}

Feature::Feature(quint64 v): _itemid(v){
    _featureid = _idbase++;
}

quint32 Feature::itemId() const {
    return _itemid;
}

void Feature::itemId(quint32 v) {
    _itemid = v;
}

quint64 Feature::featureid() const{
    return _featureid;
}


bool Feature::isValid() const {

    return _itemid != i64UNDEF ;
}

const Geometry &Feature::geometry(quint32 index) const
{
    if ( index < _track.size())
        return _track[index];
    ERROR2(ERR_INVALID_PROPERTY_FOR_2,"index","geometry");

    return _invalidGeom;
}

void Feature::add(const Geometry &geom)
{
    _track.push_back(geom );
}

bool operator==(const Feature& f1, const Feature& f2) {
    return f1.featureid() == f2.featureid();
}

void Feature::attributeRecord(const SPAttributeRecord& record){
    _record = record;
}

Ilwis::Feature *createFeature(quint64 ItemId) {
    return new Feature(ItemId);
}
//------------------------------------------------------

FeatureProxy::FeatureProxy()
{
}

FeatureProxy::~FeatureProxy()
{
}

void FeatureProxy::setProxy(SPFeature f, quint32 index) {
    _feature = f;
    _trackIndex = index;
}

QVariant FeatureProxy::value(const QString &name)
{
    if ( isValid())
        return _feature->operator [](name) ;
    return QVariant();
}

quint32 FeatureProxy::itemId() const
{
    if ( isValid())
        _feature->itemId();
    return iUNDEF;
}

void FeatureProxy::itemId(quint32 v)
{
    if ( isValid())
        _feature->itemId(v);
}

quint64 FeatureProxy::featureid() const
{
    if ( isValid())
        _feature->featureid();
    return i64UNDEF;
}

bool FeatureProxy::isValid() const
{
    return !_feature.isNull();
}

const Geometry &FeatureProxy::geometry(quint32) const
{
    if ( isValid())
        _feature->geometry(_trackIndex);
    return _invalidGeom;
}

void FeatureProxy::add(const Geometry &geom)
{
    if ( isValid())
        _feature->add(geom)  ;
}

void FeatureProxy::attributeRecord(const SPAttributeRecord &record)
{
    if ( isValid())
        _feature->attributeRecord(record) ;
}


