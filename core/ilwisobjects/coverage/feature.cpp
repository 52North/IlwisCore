#include <QStringList>
#include <QSharedPointer>
#include "kernel.h"
#include "coverage.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "vertexiterator.h"

using namespace Ilwis;

quint64 Feature::_idbase = 0;

UPFeatureI::UPFeatureI(FeatureInterface *f) : std::unique_ptr<FeatureInterface>(f)
{
}

QVariant UPFeatureI::operator ()(const QString &name, bool asRaw, int index) const {
    return (*this)->cell(name, index, asRaw);
}

void UPFeatureI::operator ()(int index, const QString &name, const QVariant &var) {
    return (*this)->setCell(name, var, index);
}

VertexIterator UPFeatureI::begin()
{
    return ::begin((*this)->geometry());
}

VertexIterator UPFeatureI::end()
{
    return ::end((*this)->geometry());
}
//--------------------------------------------
FeatureNode::FeatureNode() : _feature(0), _index(iUNDEF){

}

FeatureNode::FeatureNode(geos::geom::Geometry *geom, Feature *feature, quint32 index ) :
    _feature(feature),
    _index(index)
{
    _geometry.reset(geom);
}

quint64 FeatureNode::featureid() const {
    if ( _feature)
        return i64UNDEF;
    return _feature->featureid();
}

bool FeatureNode::isValid() const{
    return true;
}

UPGeometry& FeatureNode::geometry(quint32 ){
    return _geometry;
}
const UPGeometry& FeatureNode::geometry(quint32 ) const{
    return _geometry;
}

void FeatureNode::set(geos::geom::Geometry *geom, int ) {
    _geometry.reset(geom);
}

FeatureInterface *FeatureNode::clone() const
{
    return new FeatureNode(_geometry->clone(), _feature, _index) ;
}

IlwisTypes FeatureNode::geometryType(qint32) const{
    return FeatureCoverage::geometryType(_geometry.get());
}

quint32 FeatureNode::trackSize() const{
    return 1;
}

QVariant FeatureNode::cell(quint32 colIndex, int , bool asRaw)
{
    if ( _feature->isValid())
        return _feature->_record->cell(colIndex, _index, asRaw);
    return QVariant();

}

QVariant FeatureNode::cell(const QString& name, int, bool asRaw) {
    if ( _feature->isValid()){
        quint32 colIndex  = _feature->_record->columnIndex(name);
        return cell(colIndex, asRaw);
    }
    return QVariant();
}

void FeatureNode::setCell(const QString &name, const QVariant &var, int )
{
    quint32 colIndex  = _feature->_record->columnIndex(name);
    return setCell(colIndex, var, _index);
}

void FeatureNode::setCell(quint32 colIndex, const QVariant &var, int )
{
    if ( _feature->isValid()){
        _feature->_record->cell(colIndex, var, _index);
    }
}

ColumnDefinition FeatureNode::columndefinition(const QString &name, bool ) const{
    if (_feature->isValid())
        return _feature->columndefinition(name, false);
    else
        return ColumnDefinition();
}

ColumnDefinition FeatureNode::columndefinition(quint32 index, bool) const
{
    if (_feature->isValid())
        return _feature->columndefinition(index, false);
    else
        return ColumnDefinition();
}

quint32 FeatureNode::attributeColumnCount(bool ) const
{
    if (_feature->isValid())
        return _feature->attributeColumnCount(false);
    return iUNDEF;
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

Feature::Feature(AttributeRecord *rec) {
    _featureid = _idbase++;
    _record.reset(rec);
}

Feature::Feature(const IFeatureCoverage& fcoverage, int rec){
    _featureid = _idbase++;
    _record.reset(new AttributeRecord(rec == iUNDEF ? fcoverage->featureCount() : rec, fcoverage->attributeTable()));
}

Feature::Feature(const FeatureCoverage* fcoverage, int rec){
    _featureid = _idbase++;
    _record.reset(new AttributeRecord(rec == iUNDEF ? fcoverage->featureCount() : rec, fcoverage->attributeTable()));
}

Feature::Feature(const Feature &f) {
}

Feature &Feature::operator =(const Feature &f)
{
    return *this;
}

QVariant Feature::cell(quint32 colIndex, int index, bool asRaw)
{
    if ( index < 0){
        return _record->cell(colIndex, index, asRaw);
    }
    if ( index >= 0 && index < _track.size())
        return _track[index]->cell(colIndex, index, asRaw);
    return QVariant();//TODO: shouldn't this raise a std::out_of_range exception or similar?
}

QVariant Feature::cell(const QString &name, int index, bool asRaw)
{
    if ( index < 0){
        quint32 colIndex  = _record->columnIndex(name);
        return _record->cell(colIndex, index, asRaw);
    }
    if ( index >= 0 && index < _track.size())
        return _track[index]->cell(name, index, asRaw);
    return QVariant();//TODO: shouldn't this raise a std::out_of_range exception or similar?
}

void Feature::setCell(const QString &name, const QVariant &var, int index)
{
    if ( index < 0) {
        quint32 colIndex  = _record->columnIndex(name);
        _record->cell(colIndex, var, index);
    }
    if ( index >= 0 && index < _track.size())
        _track[index]->setCell(name,var);
}

void Feature::setCell(quint32 colIndex, const QVariant &var, int index)
{
    if ( index < 0) {
        _record->cell(colIndex, var, index);
    }
    if ( index >= 0 && index < _track.size())
        _track[index]->setCell(colIndex,var);
}

ColumnDefinition Feature::columndefinition(const QString &name, bool coverages) const{
    if (_record->isValid())
        return _record->columndefinition(name, coverages);
    else
        return ColumnDefinition();
}

ColumnDefinition Feature::columndefinition(quint32 index, bool coverages) const
{
    if (_record->isValid())
        return _record->columndefinition(index, coverages);
    else
        return ColumnDefinition();
}

quint32 Feature::attributeColumnCount(bool coverages) const
{
    if (_record->isValid())
        return _record->columnCount(coverages);
    return iUNDEF;
}

quint64 Feature::featureid() const{
    return _featureid;
}


bool Feature::isValid() const {

    return _record->isValid();
}

UPGeometry &Feature::geometry(quint32 index){
    if ( index < _track.size())
        return _track[index]->geometry();
    ERROR2(ERR_INVALID_PROPERTY_FOR_2,"index","geometry");

    throw ErrorObject(TR("Invalid geomtry index"));
}
const UPGeometry &Feature::geometry(quint32 index) const{
    if ( index < _track.size())
        return _track[index]->geometry();

    ERROR2(ERR_INVALID_PROPERTY_FOR_2,"index","geometry");

    throw ErrorObject(TR("Invalid index size in track"));
}

void Feature::set(geos::geom::Geometry *geom, int index)
{
    if ( index < _track.size())
        _track[index]->set(geom);
    else{
        UPFeatureNode node( new FeatureNode(geom, this, _track.size()));
        _track.push_back(std::move(node));
    }
}

bool operator==(const Feature& f1, const Feature& f2) {
    return f1.featureid() == f2.featureid();
}

FeatureInterface *Feature::clone() const
{
    Feature *f = new Feature(_record->clone());
    for(const UPFeatureNode& node : _track){
        UPFeatureNode ptr(static_cast<FeatureNode *>(node->clone()));
        f->_track.push_back(std::move(ptr));
    }

    return f;

}

IlwisTypes Feature::geometryType(qint32 index) const
{
    if ( index != iUNDEF ) {
        if ( index < _track.size())
            return _track[index]->geometryType();
        return itUNKNOWN;
    }
    IlwisTypes type=itUNKNOWN;
    for(const UPFeatureNode& node : _track)
        type |= node->geometryType();
    return type;
}

quint32 Feature::trackSize() const
{
    return _track.size();
}



Ilwis::FeatureInterface *createFeature(FeatureCoverage* fcoverage) {
    return new Feature(fcoverage);
}


FeatureInterface::~FeatureInterface()
{

}
