#include <QStringList>
#include <QSharedPointer>
#include "kernel.h"
#include "coverage.h"
#include "geometries.h"
#include "polygon.h"
#include "geometry.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"

using namespace Ilwis;

quint64 Feature::_idbase = 0;

SPFeatureI::SPFeatureI(FeatureInterface *f) : QSharedPointer<FeatureInterface>(f)
{
}

QVariant SPFeatureI::operator ()(const QString &name, int index, bool asRaw) {
    return (*this)->cell(name, index, asRaw);
}

void SPFeatureI::operator ()(const QString &name, const QVariant &var, int index) {
    return (*this)->setCell(name, var, index);
}
//--------------------------------------------
FeatureNode::FeatureNode() : _feature(0), _index(iUNDEF){

}

FeatureNode::FeatureNode(const Geometry& geometry, Feature *feature, quint32 index ) :
    _feature(feature),
    _geometry(geometry),
    _index(index)
{

}

quint64 FeatureNode::featureid() const {
    if ( _feature)
        return i64UNDEF;
    return _feature->featureid();
}

bool FeatureNode::isValid() const{
    return true;
}

const Geometry& FeatureNode::geometry(quint32 ) const {
    return _geometry;
}

void FeatureNode::set(const Geometry& geom, int ) {
    _geometry = geom;
}

FeatureInterface *FeatureNode::clone() const
{
    return new FeatureNode(_geometry, _feature, _index) ;
}

IlwisTypes FeatureNode::ilwisType(qint32) const{
    return _geometry.ilwisType();
}

quint32 FeatureNode::trackSize() const{
    return 1;
}

QVariant FeatureNode::cell(const QString& name, int, bool asRaw) {
    quint32 colIndex  = _feature->_record->columnIndex(name);
    return _feature->_record->cellByKey(featureid(), colIndex, _index, asRaw);
}

void FeatureNode::setCell(const QString &name, const QVariant &var, int index)
{
    quint32 colIndex  = _feature->_record->columnIndex(name);
    return _feature->_record->cellByKey(featureid(), colIndex, var, _index);
}

quint32 FeatureNode::index() const{
    return _index;
}
void FeatureNode::setIndex(quint32 ind){
    _index = ind;
}

//--------------------------------------------
Feature::Feature() : _featureid(i64UNDEF){
}

Feature::~Feature()
{
}

Feature::Feature(const SPAttributeRecord& rec) {
    _featureid = _idbase++;
    _record = rec;
}

Feature::Feature(const IFeatureCoverage& fcoverage){
    _featureid = _idbase++;
    _record = fcoverage->record();
}

Feature::Feature(const FeatureCoverage* fcoverage){
    _featureid = _idbase++;
    _record = fcoverage->record();
}

Feature::Feature(const Feature &f) {
}

Feature &Feature::operator =(const Feature &f)
{
    return *this;
}

QVariant Feature::cell(const QString &name, int index, bool asRaw)
{
    if ( index < 0){
        quint32 colIndex  = _record->columnIndex(name);
        return _record->cellByKey(featureid(), colIndex, index, asRaw);
    }
    if ( index >= 0 && index < _track.size())
        return _track[index]->cell(name, index, asRaw);
    return QVariant();//TODO shouldn't this raise a std::out_of_range exception or similar?
}

void Feature::setCell(const QString &name, const QVariant &var, int index)
{
    if ( index < 0) {
        quint32 colIndex  = _record->columnIndex(name);
        _record->cellByKey(featureid(), colIndex, var, index);
    }
    if ( index >= 0 && index < _track.size())
        _track[index]->setCell(name,var);
}

quint64 Feature::featureid() const{
    return _featureid;
}


bool Feature::isValid() const {

    return _record->isValid();
}

const Geometry &Feature::geometry(quint32 index) const
{
    if ( index < _track.size())
        return _track[index]->geometry();
    ERROR2(ERR_INVALID_PROPERTY_FOR_2,"index","geometry");

    return _invalidGeom;
}

void Feature::set(const Geometry &geom, int index)
{
    if ( index < _track.size())
        _track[index]->set(geom);
    else{
        SPFeatureNode node( new FeatureNode(geom, this, _track.size()));
        _track.push_back(node);
    }
}

bool operator==(const Feature& f1, const Feature& f2) {
    return f1.featureid() == f2.featureid();
}

FeatureInterface *Feature::clone() const
{
    Feature *f = new Feature(_record);
    for(const SPFeatureNode& node : _track){
        SPFeatureNode ptr(static_cast<FeatureNode *>(node->clone()));
        f->_track.push_back(ptr);
    }
    f->_record = _record;

    return f;

}

IlwisTypes Feature::ilwisType(qint32 index) const
{
    if ( index != iUNDEF ) {
        if ( index < _track.size())
            return geometry(index).ilwisType();
        return itUNKNOWN;
    }
    IlwisTypes type=itUNKNOWN;
    for(const SPFeatureNode& node : _track)
        type |= node->geometry().ilwisType();
    return type;
}

quint32 Feature::trackSize() const
{
    return _track.size();
}

Ilwis::FeatureInterface *createFeature(FeatureCoverage* fcoverage) {
    return new Feature(fcoverage);
}
