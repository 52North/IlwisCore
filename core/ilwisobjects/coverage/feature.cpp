#include <QStringList>
#include <QSharedPointer>
#include "coverage.h"
#include "table.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "record.h"
#include "featurecoverage.h"
#include "feature.h"
#include "vertexiterator.h"
#include "geometryhelper.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/Point.h"
#include "geos/geom/CoordinateArraySequence.h"

using namespace Ilwis;

quint64 Feature::_idbase = 0;

SPFeatureI::SPFeatureI(FeatureInterface *f) : std::shared_ptr<FeatureInterface>(f)
{
}

QVariant SPFeatureI::operator ()(const QString &attributeName) const {
    return (*this)->cell(attributeName);
}

void SPFeatureI::operator ()(const QString &columnName, const QVariant &var) {
    if (get())
        return (*this)->setCell(columnName, var);
}

QVariant SPFeatureI::operator ()(quint32 colIndex) const {
    if (!get())
        return QVariant();
    return (*this)->cell(colIndex);
}

void SPFeatureI::operator ()(quint32 colIndex, const QVariant &var) {
    if (!get())
        return;
    return (*this)->setCell(colIndex, var);
}

SPFeatureI& SPFeatureI::operator[](const QString &subFeatureIndex)
{
    if (!get())
        throw ErrorObject(TR("Using uninitialized feature or sub feature, are the attribute names correct?"));
    return (*this)->subFeatureRef(subFeatureIndex);
}

const SPFeatureI& SPFeatureI::operator[](const QString &subFeatureIndex) const
{
    if (!get())
        throw ErrorObject(TR("Using uninitialized feature or sub feature, are the attribute names correct?"));
    return (*this)->subFeatureRef(subFeatureIndex);
}

SPFeatureI& SPFeatureI::operator[]( quint32 subFeatureIndex)
{
    return (*this)->subFeatureRef(subFeatureIndex);
}

const SPFeatureI& SPFeatureI::operator[]( quint32 subFeatureIndex) const
{
    if (!get())
        throw ErrorObject(TR("Using uninitialized feature or sub feature, are the attribute names correct?"));
    return (*this)->subFeatureRef(subFeatureIndex);
}

//SPFeatureI &SPFeatureI::operator =(const SPFeatureI &otherFeature)
//{
//    reset(otherFeature->clone());
//    return *this;
//}

VertexIterator SPFeatureI::begin()
{
    if (!get())
        return VertexIterator();
    return ::begin((*this)->geometry());
}

VertexIterator SPFeatureI::end()
{
    if (!get())
        return VertexIterator();
    return ::end((*this)->geometry());
}

//--------------------------------------------
Feature::Feature() : _featureid(i64UNDEF){
}

Feature::~Feature()
{
}

Feature::Feature(const IFeatureCoverage& fcoverage, qint32 level){
    _featureid = _idbase++;
    _parentFCoverage = fcoverage;
    _attributes.addColumn(_parentFCoverage->attributeDefinitions().definitionCount());
    _level = level;
}

Feature::Feature(FeatureCoverage *fcoverage, qint32 level)
{
    _featureid = _idbase++;
    _parentFCoverage = fcoverage;
    _parentFCoverage.set(fcoverage)    ;
    _attributes.addColumn(_parentFCoverage->attributeDefinitions().definitionCount());
    _level = level;
}

Feature::Feature(const Feature &f) {
}

Feature &Feature::operator =(const Feature &f)
{
    return *this;
}

Record &Feature::recordRef()
{
    if ( isValid() ){
       return _attributes;
    }
    throw ErrorObject(TR(QString("feature is not valid")));
}

const Record &Feature::record() const
{
    if ( isValid() ){
       return _attributes;
    }
    throw ErrorObject(TR(QString("feature is not valid")));
}

void Feature::record(const std::vector<QVariant> &values, quint32 offset)
{
    if ( isValid() && (values.size() + offset) == _parentFCoverage->attributeDefinitions(_level).definitionCount()){
        for(int i = offset; i < values.size(); ++i) {
            QVariant value = _parentFCoverage->attributeDefinitions(_level).checkInput(values[i],i);
            _attributes.cell(i, value);
        }

    }
}

QVariant Feature::cell(quint32 colIndex, bool asRaw) const
{
    if ( isValid() && colIndex < _parentFCoverage->attributeDefinitions(_level).definitionCount()){
        QVariant cellValue = _attributes.cell(colIndex);
        if ( asRaw)
            return cellValue;
        if ( !asRaw) {
            ColumnDefinition attributedef = _parentFCoverage->attributeDefinitions(_level).columndefinition(colIndex);
            return attributedef.datadef().domain<>()->impliedValue(cellValue);
        }
    }
    return QVariant();
}

QVariant Feature::cell(const QString &columnname, bool asRaw) const
{
    if ( isValid()){
        int columnIndex =_parentFCoverage->attributeDefinitions(_level).columnIndex(columnname);
        return cell(columnIndex, asRaw);
    }
    return QVariant();
}

void Feature::setCell(const QString &columnname, const QVariant &var)
{
    if ( isValid()){
        int columnIndex =_parentFCoverage->attributeDefinitions(_level).columnIndex(columnname);
        setCell(columnIndex, var);
    }
}

void Feature::setCell(quint32 colIndex, const QVariant &var)
{
    QVariant value = _parentFCoverage->attributeDefinitions(_level).checkInput(var,colIndex);
    _attributes.cell(colIndex, value);
}

ColumnDefinition Feature::attributedefinition(const QString &attributeName) const{
    int columnIndex =_parentFCoverage->attributeDefinitions(_level).columnIndex(attributeName);
    return _parentFCoverage->attributeDefinitions(_level).columndefinition(columnIndex);
}

ColumnDefinition Feature::attributedefinition(quint32 columnIndex) const
{
    return _parentFCoverage->attributeDefinitions(_level).columndefinition(columnIndex);
}

quint32 Feature::attributeColumnCount() const
{
    return _parentFCoverage->attributeDefinitions(_level).definitionCount();
}

SPFeatureI& Feature::subFeatureRef(double subFeatureIndex)
{
    quint32 index = _parentFCoverage->attributeDefinitions(_level).index(subFeatureIndex);
    if ( index != iUNDEF)
        return _subFeature[index];
    throw ErrorObject(TR("Illegal value for variant selection"));
}

SPFeatureI& Feature::subFeatureRef(const QString &subFeatureIndex)
{
    quint32 index = _parentFCoverage->attributeDefinitions(_level).index(subFeatureIndex);
    if ( index != iUNDEF){
        return _subFeature[index];
    }
    throw ErrorObject(TR("Illegal value for variant selection"));
}

void Feature::store(QDataStream &stream)
{
//    stream << _featureid;
//    stream << _track.size();
//    for(const UPFeatureI& feature : _track){
//        feature->store(stream);
//    }
}

void Feature::load(QDataStream &stream, const UPGeomFactory &factory)
{
//    stream >> _featureid;
//    int sz;
//    stream >> sz;
//    for(int i = 0; i < sz; ++i){
//        UPFeatureI geomnode(new GeometryNode(0,this, i));
//        geomnode->load(stream, factory);
//        ITable indexTable = _parentFCoverage->attributeTable(Coverage::atINDEX);
//        indexTable->record(NEW_RECORD,{featureid(),_track.size(), i});
//        _parentFCoverage->setFeatureCount(GeometryHelper::geometryType(geomnode->geometry().get()),1,geomnode->geometry()->getNumGeometries() );
//        _parentFCoverage->indexDefinition().addIndex(featureid(),_track.size(), 0);
//        _track.push_back(std::move(geomnode));
//    }
}

quint64 Feature::featureid() const{
    return _featureid;
}


bool Feature::isValid() const {

    return _attributes.isValid() || _geometry;
}

UPGeometry &Feature::geometryRef(){
    return _geometry;
}

const UPGeometry &Feature::geometry() const{
    return _geometry;
}

void Feature::geometry(geos::geom::Geometry *geom){
    _geometry.reset(geom);
}

void Feature::removeSubFeature(const QString &subFeatureIndex)
{
    removeSubFeaturePrivate(subFeatureIndex);
}

void Feature::removeSubFeature(double subFeatureIndex)
{
    removeSubFeaturePrivate(subFeatureIndex);
}

void Feature::setSubFeature(const QString &subFeatureIndex, FeatureInterface *feature)
{
    setSubFeaturePrivate(subFeatureIndex, feature);
}

void Feature::setSubFeature(const QString &subFeatureIndex, SPFeatureI &feature)
{
    setSubFeaturePrivate(subFeatureIndex, feature->clone());
}

void Feature::setSubFeature(double subFeatureIndex, FeatureInterface *feature)
{
    setSubFeaturePrivate(subFeatureIndex, feature);
}

void Feature::setSubFeature(double subFeatureIndex, SPFeatureI &feature)
{
   setSubFeaturePrivate(subFeatureIndex, feature->clone());
}

quint32 Feature::subFeatureCount() const
{
    return _subFeature.size();
}


bool operator==(const Feature& f1, const Feature& f2) {
    return f1.featureid() == f2.featureid();
}

FeatureInterface *Feature::clone() const
{
    Feature *f = new Feature(_parentFCoverage);
    for(const auto& node : _subFeature){
        f->_subFeature[node.first].reset(node.second->clone());
    }
    f->_parentFCoverage = _parentFCoverage;
    if ( _geometry)
        f->_geometry.reset(_geometry->clone());
    f->_attributes = _attributes;
    f->_level = _level;

    return f;

}

SPFeatureI Feature::createSubFeature(const QString& subFeatureIndex, geos::geom::Geometry *geom)
{
    quint32 index = _parentFCoverage->attributeDefinitions().index(subFeatureIndex);
    if ( index == iUNDEF)
        return SPFeatureI();

    IlwisTypes tp = GeometryHelper::geometryType(geom);
    _parentFCoverage->setFeatureCount(tp,1);
    CreateFeature create = _parentFCoverage->_featureFactory->getCreator("feature");
    _subFeature[index] = create(_parentFCoverage, _level + 1);
    _subFeature[index]->geometry(geom);

    return _subFeature[index];
}

IlwisTypes Feature::geometryType() const
{
    if (!_geometry)
        return itUNKNOWN;
    return GeometryHelper::geometryType(_geometry.get());
}

Ilwis::FeatureInterface *createFeature(const IFeatureCoverage& fcoverage, qint32 level) {
    return new Feature(fcoverage, level);
}


