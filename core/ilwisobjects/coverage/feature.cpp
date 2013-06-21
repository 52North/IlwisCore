#include <QStringList>
#include <QSharedPointer>
#include "kernel.h"
#include "geometries.h"
#include "geometry.h"
#include "ilwisdata.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"

using namespace Ilwis;

quint64 Feature::_idbase = 0;

QVariant FeatureInterface::operator ()(const QString &name, int index) {
    return value(name, index);
}

//--------------------------------------------
Feature::Feature() : _itemid(iUNDEF), _featureid(i64UNDEF){
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

QVariant Feature::value(const QString &name, int index)
{
    return _record->cellByKey(itemId(), name, index);
}

Feature::Feature(quint32 v): _itemid(v){
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

    return _itemid != iUNDEF ;
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

SPFeatureI Feature::clone() const
{
    Feature *f = new Feature(itemId());
    f->_track = _track;
    f->_record = _record;

    return SPFeatureI(f);

}

IlwisTypes Feature::ilwisType(qint32 index) const
{
    if ( index != iUNDEF ) {
        if ( index < _track.size())
            return geometry(index).ilwisType();
        return itUNKNOWN;
    }
    IlwisTypes type=itUNKNOWN;
    for(const Geometry& geom : _track)
        type |= geom.ilwisType();
    return type;
}

quint32 Feature::trackSize() const
{
    return _track.size();
}

Ilwis::FeatureInterface *createFeature(quint64 ItemId) {
    return new Feature(ItemId);
}
//------------------------------------------------------

FeatureProxy::FeatureProxy(bool illegal) : _illegal(illegal)
{
}

FeatureProxy::~FeatureProxy()
{
}

void FeatureProxy::setProxy(const SPFeatureI& f, quint32 index) {
    _feature = f;
    _trackIndex = index;
}

SPFeatureI FeatureProxy::clone() const
{
    if ( _feature.isNull())
        return SPFeatureI();

    return _feature->clone();
}

QVariant FeatureProxy::value(const QString &name, int index)
{
    if ( isValid())
        return _feature->operator ()(name, index) ;
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
    if ( _illegal)
        return false;

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

IlwisTypes FeatureProxy::ilwisType(qint32 ) const
{

    return _feature->geometry(_trackIndex).ilwisType();
}

quint32 FeatureProxy::trackSize() const
{
    return 1;
}


