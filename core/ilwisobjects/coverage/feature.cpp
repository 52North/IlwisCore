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
#include "geometryhelper.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/Point.h"
#include "geos/geom/CoordinateArraySequence.h"

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
GeometryNode::GeometryNode() : _feature(0), _trackIndex(iUNDEF){

}

GeometryNode::GeometryNode(geos::geom::Geometry *geom, Feature *feature, quint32 index ) :
    _feature(feature),
    _trackIndex(index)
{
    _geometry.reset(geom);
}

quint64 GeometryNode::featureid() const {
    if ( _feature)
        return i64UNDEF;
    return _feature->featureid();
}

bool GeometryNode::isValid() const{
    return true;
}

UPGeometry& GeometryNode::geometry(const QVariant & ){
    return _geometry;
}

void GeometryNode::set(geos::geom::Geometry *geom, const QVariant&)
{
    _geometry.reset(geom);
}
const UPGeometry& GeometryNode::geometry(const QVariant & ) const{
    return _geometry;
}

FeatureInterface *GeometryNode::clone() const
{
    return new GeometryNode(_geometry->clone(), _feature, _trackIndex) ;
}

IlwisTypes GeometryNode::geometryType(qint32) const{
    return FeatureCoverage::geometryType(_geometry.get());
}

quint32 GeometryNode::trackSize() const{
    return 1;
}

std::vector<QVariant> GeometryNode::record(const QVariant &trackIndexValue) const
{
    if ( _feature->isValid() ){
        if (trackIndexValue == QVariant())
            return _feature->_record->record(-1);
        //TODO index variant
    }
    return std::vector<QVariant>();
}

QVariant GeometryNode::cell(quint32 colIndex, const QVariant & , bool asRaw)
{
    if ( _feature->isValid())
        return _feature->_record->cell(colIndex, _trackIndex, asRaw);
    return QVariant();

}

QVariant GeometryNode::cell(const QString& name, const QVariant &, bool asRaw) {
    if ( _feature->isValid()){
        quint32 colIndex  = _feature->_record->columnIndex(name);
        return cell(colIndex, asRaw);
    }
    return QVariant();
}

void GeometryNode::setCell(const QString &name, const QVariant &var, const QVariant& )
{
    quint32 colIndex  = _feature->_record->columnIndex(name);
    return setCell(colIndex, var, _trackIndex);
}

void GeometryNode::setCell(quint32 colIndex, const QVariant &var, const QVariant & )
{
    if ( _feature->isValid()){
        _feature->_record->cell(colIndex, var, _trackIndex);
    }
}

void GeometryNode::record(const std::vector<QVariant> &values, const QVariant &trackIndexValue)
{
    if ( _feature->isValid() ){
        if (trackIndexValue == QVariant())
            return _feature->_record->record(values);
        //TODO index variant
    }
}

ColumnDefinition GeometryNode::columndefinition(const QString &name, bool ) const{
    if (_feature->isValid())
        return _feature->columndefinition(name, false);
    else
        return ColumnDefinition();
}

ColumnDefinition GeometryNode::columndefinition(quint32 index, bool) const
{
    if (_feature->isValid())
        return _feature->columndefinition(index, false);
    else
        return ColumnDefinition();
}

quint32 GeometryNode::attributeColumnCount(bool ) const
{
    if (_feature->isValid())
        return _feature->attributeColumnCount(false);
    return iUNDEF;
}

quint32 GeometryNode::trackIndex() const{
    return _trackIndex;
}
void GeometryNode::setTrackIndex(quint32 ind){
    _trackIndex = ind;
}

void GeometryNode::store(QDataStream &stream)
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

void GeometryNode::load(QDataStream &stream, const UPGeomFactory &factory)
{
    int gtype;
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

}

//--------------------------------------------
Feature::Feature() : _featureid(i64UNDEF){
}

Feature::~Feature()
{
}

Feature::Feature(FeatureCoverage* fcoverage, AttributeRecord *rec) {
    _featureid = _idbase++;
    _record.reset(rec);
}

Feature::Feature(IFeatureCoverage& fcoverage, int rec){
    _featureid = _idbase++;
    _record.reset(new AttributeRecord(rec == iUNDEF ? fcoverage->featureCount() : rec, fcoverage->attributeTable(),fcoverage->attributeTable(Coverage::atINDEX)));
    _parentFCoverage = fcoverage.ptr();
}

Feature::Feature(FeatureCoverage* fcoverage, int rec){
    _featureid = _idbase++;
    _record.reset(new AttributeRecord(rec == iUNDEF ? fcoverage->featureCount() : rec, fcoverage->attributeTable(),fcoverage->attributeTable(Coverage::atINDEX)));
    _parentFCoverage = fcoverage;
}

Feature::Feature(const Feature &f) {
}

Feature &Feature::operator =(const Feature &f)
{
    return *this;
}

std::pair<quint32, quint32> Feature::getIndexes(const QVariant &trackIndexValue) const
{
    quint32 recordIndex = trackIndexValue == COVERAGEATRIB ? COVERAGEATRIB : _parentFCoverage->indexDefinition()(featureid(), trackIndexValue);
    ITable& indexTable = _parentFCoverage->attributeTable(Coverage::atINDEX);
    quint32 trackIndex = 0;
    if ( recordIndex < indexTable->recordCount())
        trackIndex = indexTable->cell(TRACKINDEXCOLUMN,recordIndex).toUInt();
    return std::pair<quint32, quint32>(recordIndex, trackIndex);
}

std::vector<QVariant> Feature::record(const QVariant &trackIndexValue) const
{
    if ( isValid() ){
        if (trackIndexValue == COVERAGEATRIB)
            return _record->record(-1);
        //TODO index variant
    }
    return std::vector<QVariant>();
}

void Feature::record(const std::vector<QVariant> &values, const QVariant &trackIndexValue)
{
    if ( isValid() ){
        if (trackIndexValue == COVERAGEATRIB)
            return _record->record(values);
        //TODO index variant
    }
}

QVariant Feature::cell(quint32 colIndex, const QVariant &trackIndexValue, bool asRaw)
{
    if ( trackIndexValue == COVERAGEATRIB){
        return _record->cell(colIndex, COVERAGEATRIB, asRaw);
    }
    TrackIndexes indexes = getIndexes(trackIndexValue);
    if ( indexes.second < _track.size())
        return _track[indexes.second]->cell(colIndex, indexes.first, asRaw);
    return QVariant();
}

QVariant Feature::cell(const QString &name, const QVariant &trackIndexValue, bool asRaw)
{
    if ( trackIndexValue  == COVERAGEATRIB){
        quint32 colIndex  = _record->columnIndex(name);
        return _record->cell(colIndex, COVERAGEATRIB, asRaw);
    }
    TrackIndexes indexes = getIndexes(trackIndexValue);
    if ( indexes.second < _track.size())
        return _track[indexes.second]->cell(name, indexes.first , asRaw);
    return QVariant();
}

void Feature::setCell(const QString &name, const QVariant &var, const QVariant &trackIndexValue)
{
    if ( trackIndexValue == COVERAGEATRIB) {
        quint32 colIndex  = _record->columnIndex(name);
        _record->cell(colIndex, var, COVERAGEATRIB);
    } else {
        TrackIndexes indexes = getIndexes(trackIndexValue);
        if ( indexes.second < _track.size())
            _track[indexes.second]->setCell(name,var, indexes.first);
    }
}

void Feature::setCell(quint32 colIndex, const QVariant &var, const QVariant &trackIndexValue)
{
    if ( trackIndexValue == COVERAGEATRIB) {
        _record->cell(colIndex, var, COVERAGEATRIB);
    }else {
        TrackIndexes indexes = getIndexes(trackIndexValue);
        if ( indexes.second < _track.size())
            _track[indexes.second]->setCell(colIndex,var);
    }

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

QVariant Feature::trackIndexValue(quint32 index) const
{
    if ( index < _track.size())
        return _track[index]->cell(TRACKVALUECOLUMN);
    return QVariant();
}

void Feature::store(QDataStream &stream)
{
    stream << _featureid;
    stream << _track.size();
    for(const UPFeatureI& feature : _track){
        feature->store(stream);
    }
}

void Feature::load(QDataStream &stream, const UPGeomFactory &factory)
{
    stream >> _featureid;
    int sz;
    stream >> sz;
    for(int i = 0; i < sz; ++i){
        UPFeatureI geomnode(new GeometryNode(0,this, i));
        geomnode->load(stream, factory);
        _track.push_back(std::move(geomnode));
    }
}

quint64 Feature::featureid() const{
    return _featureid;
}


bool Feature::isValid() const {

    return _record->isValid();
}

UPGeometry &Feature::geometry(const QVariant &trackIndexValue){
    if ( !trackIndexValue.isValid() && _track.size() == 1)
        return _track[0]->geometry();
    TrackIndexes indexes = getIndexes(trackIndexValue);
    if ( indexes.second < _track.size())
        return _track[indexes.second]->geometry();

    throw ErrorObject(TR("Invalid geomtry index"));
}
const UPGeometry &Feature::geometry(const QVariant &trackIndexValue) const{
    if ( !trackIndexValue.isValid() && _track.size() == 1)
        return _track[0]->geometry();

     TrackIndexes indexes = getIndexes(trackIndexValue);
    if ( indexes.second < _track.size())
        return _track[indexes.second]->geometry();

    throw ErrorObject(TR("Invalid index size in track"));
}

void Feature::add(geos::geom::Geometry *geom, const QVariant &trackIndexValue){

    auto createNode = [&](UPFeatureI& node, const QVariant& value){
        ITable& indexTable = _parentFCoverage->attributeTable(Coverage::atINDEX);
        indexTable->record(NEW_RECORD,{featureid(),_track.size(), value});
        node.reset( new GeometryNode(geom, this, _track.size()));
        _parentFCoverage->setFeatureCount(GeometryHelper::geometryType(geom),1,geom->getNumGeometries() );
        _parentFCoverage->indexDefinition().addIndex(featureid(),value, 0);
    };

    if (!geom )
        return;
    if ( !trackIndexValue.isValid() ) {// special case, all feature with one geometry
        UPFeatureI node;
        createNode(node, 0); // trackindex is 0 as this is the default 1 track size case
        _track.resize(1);
        _track[0] = std::move(node);
    } else if(_parentFCoverage->indexDefinition().domain()->contains(trackIndexValue)){
        QVariant value = _parentFCoverage->indexDefinition().domain()->impliedValue(trackIndexValue);
        if ( !value.isValid())
            return;
        UPFeatureI node; //( new GeometryNode(geom, this, _track.size()));
        createNode(node, value);
        _track.push_back(std::move(node));
    }
}

void Feature::remove(const QVariant& trackIndexValue)
{
    TrackIndexes indexes = getIndexes(trackIndexValue);
    _parentFCoverage->setFeatureCount(geometryType(indexes.second),-1,-_track[indexes.second]->geometry()->getNumGeometries(),indexes.second );
    _track.erase(_track.begin() + indexes.second);
    _parentFCoverage->attributeTable(Coverage::atINDEX)->removeRecord(indexes.first);
    _parentFCoverage->indexDefinition().removeIndex(featureid(),indexes.second);
}

void Feature::set(geos::geom::Geometry *geom, const QVariant &trackIndexValue)
{
    TrackIndexes indexes = getIndexes(trackIndexValue);
    _parentFCoverage->setFeatureCount(geometryType(indexes.second),indexes.second,-_track[indexes.second]->geometry()->getNumGeometries() );
    _track[indexes.second]->set(geom);
    _parentFCoverage->setFeatureCount(geometryType(indexes.second),indexes.second,_track[indexes.second]->geometry()->getNumGeometries() );
}


bool operator==(const Feature& f1, const Feature& f2) {
    return f1.featureid() == f2.featureid();
}

FeatureInterface *Feature::clone() const
{
    Feature *f = new Feature(_parentFCoverage, _record->clone());
    for(const UPFeatureI& node : _track){
        UPFeatureI ptr(static_cast<GeometryNode *>(node->clone()));
        f->_track.push_back(std::move(ptr));
    }
    f->_parentFCoverage = _parentFCoverage;

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
    for(const UPFeatureI& node : _track)
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
