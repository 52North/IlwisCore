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

void Feature::store(const FeatureAttributeDefinition& columns, QDataStream &stream, const IOOptions &options)
{
    std::vector<IlwisTypes> types = columns.ilwisColumnTypes();
    _attributes.storeData(types,stream,options);
    storeGeometry(stream);
    stream << _subFeature.size();
    for(auto iter= _subFeature.begin(); iter != _subFeature.end(); ++iter){
        stream << (*iter).first;
        SPFeatureI subfeature = (*iter).second;
        subfeature->store(columns.featureAttributeDefinition(),stream, options);
    }
}

void Feature::storeGeometry(QDataStream &stream)
{
    auto StoreSequence = [&] (geos::geom::CoordinateSequence *crds, QDataStream& stream){
        stream << crds->size();
        for(int i = 0; i < crds->size(); ++i)
            stream << crds->getAt(i).x << crds->getAt(i).y << crds->getAt(i).z;
        delete crds;
    };

    auto StorePolygon = [&] (const geos::geom::Geometry* geom,QDataStream& stream ){
        const geos::geom::Polygon* polygon = dynamic_cast<const geos::geom::Polygon *>(geom);
        StoreSequence(polygon->getExteriorRing()->getCoordinates(), stream);
        stream << polygon->getNumInteriorRing();
        for(int g = 0; g < polygon->getNumInteriorRing(); ++g){
            StoreSequence(polygon->getInteriorRingN(g)->getCoordinates(), stream);
        }
    };

    int gtype = _geometry->getGeometryTypeId();
    stream << gtype;
    if ( gtype == geos::geom::GEOS_POINT || gtype == geos::geom::GEOS_MULTIPOINT || gtype == geos::geom::GEOS_LINESTRING){
        StoreSequence(_geometry->getCoordinates(), stream);
    } else if ( gtype == geos::geom::GEOS_MULTILINESTRING){
        stream << _geometry->getNumGeometries();
        for(int g =0; g < _geometry->getNumGeometries(); ++g){
            const geos::geom::Geometry *geom = _geometry->getGeometryN(g);
            StoreSequence(geom->getCoordinates(), stream);
        }
    } else if (gtype == geos::geom::GEOS_POLYGON ) {
        const geos::geom::Geometry *gm = _geometry.get();
        StorePolygon(gm, stream);
    } else if (gtype == geos::geom::GEOS_MULTIPOLYGON) {
        const geos::geom::Geometry *gm = _geometry.get();
        stream << gm->getNumGeometries();
        for(int g = 0; g < gm->getNumGeometries(); ++g ){
            StorePolygon(gm->getGeometryN(g), stream);
        }
    }
}

void Feature::loadGeometry(QDataStream &stream)
{
    int gtype;
    const UPGeomFactory &factory = _parentFCoverage->geomfactory();
    stream >> gtype;

    auto LoadSequence = [&] (QDataStream& stream)-> geos::geom::CoordinateSequence *{
        quint32 nrOfCoords;
        stream >> nrOfCoords;
        std::vector<geos::geom::Coordinate> *coords = new std::vector<geos::geom::Coordinate>(nrOfCoords) ;
        for(int i = 0; i < nrOfCoords; ++i){
            double x,y,z;
            stream >> x >> y >> z;
            coords->at(i) = geos::geom::Coordinate(x,y,z);
        }
        geos::geom::CoordinateArraySequence *seq = new geos::geom::CoordinateArraySequence(coords);
        return seq;
    };

    auto LoadPolygon = [&] (QDataStream& stream )-> geos::geom::Polygon * {
        geos::geom::CoordinateSequence *seq = LoadSequence(stream);
        geos::geom::LinearRing *outerring = factory->createLinearRing(seq);
        quint32 numInteriorRings;
        stream >> numInteriorRings;
        std::vector<geos::geom::Geometry*> *inners = new std::vector<geos::geom::Geometry*>(numInteriorRings);
        for(int g = 0; g < numInteriorRings; ++g){
            geos::geom::CoordinateSequence *seq = LoadSequence(stream);
            (*inners)[g] = factory->createLinearRing(seq);
        }
        geos::geom::Polygon *pol = factory->createPolygon(outerring, inners);
        return pol;
    };

    if ( gtype == geos::geom::GEOS_POINT){
        geos::geom::CoordinateSequence *seq = LoadSequence(stream);
        _geometry.reset( factory->createPoint(seq));
    }else if ( gtype == geos::geom::GEOS_MULTIPOINT){
         geos::geom::CoordinateSequence *seq = LoadSequence(stream);
         _geometry.reset(factory->createMultiPoint(*seq));
         delete seq;
    }else if ( gtype == geos::geom::GEOS_LINESTRING){
        geos::geom::CoordinateSequence *seq = LoadSequence(stream);
        _geometry.reset( factory->createLineString(seq));
    } else if ( gtype == geos::geom::GEOS_MULTILINESTRING){
        quint32 numSubGeoms;
        stream >> numSubGeoms;
        std::vector<geos::geom::Geometry*> *subgeoms = new std::vector<geos::geom::Geometry*>(numSubGeoms);
        for(int g =0; g < numSubGeoms; ++g){
            geos::geom::CoordinateSequence *seq = LoadSequence(stream);
            (*subgeoms)[g] = factory->createLineString(seq);
        }
        _geometry.reset(factory->createMultiLineString(subgeoms));
    } else if (gtype == geos::geom::GEOS_POLYGON ) {
        _geometry.reset(LoadPolygon(stream));
    } else if (gtype == geos::geom::GEOS_MULTIPOLYGON) {
        quint32 numSubGeoms;
        stream >> numSubGeoms;
        std::vector<geos::geom::Geometry*> *subgeoms = new std::vector<geos::geom::Geometry*>(numSubGeoms);
        for(int g = 0; g < numSubGeoms; ++g ){
            (*subgeoms)[g] = LoadPolygon(stream);
        }
        _geometry.reset(factory->createMultiPolygon(subgeoms));
    }
    _parentFCoverage->setFeatureCount( GeometryHelper::geometryType(_geometry.get()),1);

}

void Feature::load(const FeatureAttributeDefinition& columns, QDataStream &stream, const IOOptions &options)
{

    std::vector<IlwisTypes> types = columns.ilwisColumnTypes();
    size_t size;
    _attributes.loadData(types, stream,options);
    loadGeometry(stream);
    stream >> size;
    for(int i = 0; i < size; ++i){
        quint32 index;
        stream >> index;
        CreateFeature create = _parentFCoverage->_featureFactory->getCreator("feature");
        FeatureInterface *feature = create(_parentFCoverage, _level + 1);
        feature->load(columns.featureAttributeDefinition(), stream, options);
        _subFeature[index].reset(feature);

    }
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


