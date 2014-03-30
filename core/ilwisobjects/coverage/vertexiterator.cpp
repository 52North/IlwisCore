#include "geos/geom/Geometry.h"
#include "geos/geom/Coordinate.h"
//#include "geos/geom/Coordinate.inl"
#include "geos/geom/LineString.h"
#include "geos/geom/point.h"
#include "geos/geom/Polygon.h"
#include "geos/io/ParseException.h"
#include "kernel.h"
#include "errorobject.h"
#include "coverage.h"
#include "feature.h"
#include "featurecoverage.h"
#include "vertexiterator.h"
#include "geometryhelper.h"

using namespace Ilwis;

VertexIterator::VertexIterator()
{
}

VertexIterator::VertexIterator(const UPGeometry& geom)
{
    setFromGeometry(geom.get());
}

VertexIterator::VertexIterator(const QString &wkt)
{
    geos::geom::Geometry *geom = GeometryHelper::fromWKT(wkt.toStdString());
    setFromGeometry(geom);
    _internalGeom.reset(geom);
}

VertexIterator::VertexIterator(const VertexIterator &iter)
{
    operator=(iter)    ;
}

VertexIterator::~VertexIterator()
{
}

VertexIterator::VertexIterator(geos::geom::Geometry *geom)
{
    setFromGeometry(geom);
}

VertexIterator &VertexIterator::operator=(const VertexIterator &iter)
{
    _coordinates = iter._coordinates;
    _index = iter._index;
    _partIndex = iter._partIndex;
    _pointCoordinates = iter._pointCoordinates;
    _pointMode = iter._pointMode;
    _hasOwnership = iter._hasOwnership;
    if ( iter._internalGeom)
        _internalGeom.reset(iter._internalGeom->clone());
    else
        _internalGeom.reset(0);

    return *this;
}

void VertexIterator::storeLineString(const geos::geom::LineString *cline, int index){
    const geos::geom::CoordinateSequence *seq = cline->getCoordinatesRO();
     _coordinates[index] = seq;
    _linearSize += seq->size();
}

VertexIterator &VertexIterator::operator++()
{
    move(1);
    return *this;
}

VertexIterator VertexIterator::operator++(int n)
{
    VertexIterator iter = *this;
    move(n);
    return iter;
}

VertexIterator &VertexIterator::operator--()
{
    move(-1);
    return *this;
}

VertexIterator VertexIterator::operator--(int n)
{
    VertexIterator iter = *this;
    move(-n);
    return iter ;
}

VertexIterator &VertexIterator::operator+=(int n)
{
    move(n);
    return *this;
}

VertexIterator &VertexIterator::operator-=(int n)
{
    move(-n);
    return *this;
}

geos::geom::Coordinate &VertexIterator::operator[](quint32 n)
{
    if ( n < _linearSize){
        int total = 0;
        int localIndex = 0;
        if ( _pointMode){
            return *const_cast<geos::geom::Coordinate*>(_pointCoordinates[n]);
        }
        else{
            for(auto& vec : _coordinates){
                if ( total + n >= vec->size()){
                    return const_cast<geos::geom::Coordinate&>(_coordinates[_partIndex]->getAt(n - total));
                }
                total += n;
                ++localIndex;
            }
        }
    }
    throw ErrorObject(TR("vertex index out of range"));
}

bool VertexIterator::operator==(const VertexIterator &iter) const
{
    if ( _index != iter._index)
        return false;
    if ( _partIndex !=  iter._partIndex)
        return false;
    if (!compatible(iter))
        return false;
    if ( iter._partIndex  == _partIndex && _partIndex == _coordinates.size()){
        return true;    // end condition;
    }
    int j = 0;
    if ( _pointMode){
         for(int i=0; i < _pointCoordinates.size(); ++i){
             if(_pointCoordinates[i] != iter._pointCoordinates[i])
                 return false;
         }
         return true;
    }
    for(const auto& vec : _coordinates){

        for( int  i = 0; i <  vec->size(); ++i){
            if ( vec->getAt(i) != iter._coordinates[j]->getAt(i))
                return false;
        }
    }
    return true;
}

bool VertexIterator::operator!=(const VertexIterator &iter) const
{
    bool ok = ! operator==(iter);

    return ok;
}

bool VertexIterator::operator<(const VertexIterator &iter) const
{
    if(!compatible(iter))
        return false;
    if ( _partIndex > iter._partIndex)
        return false;
    return _index < iter._index;
}

bool VertexIterator::operator>(const VertexIterator &iter) const
{
    if(!compatible(iter))
        return false;
    if ( _partIndex < iter._partIndex)
        return false;
    return _index > iter._index;
}

bool VertexIterator::operator<=(const VertexIterator &iter) const
{
    if(!compatible(iter))
        return false;
    if ( _partIndex < iter._partIndex)
        return false;
    return _index >= iter._index;
}

bool VertexIterator::operator>=(const VertexIterator &iter) const
{
    if(!compatible(iter))
        return false;
    if ( _partIndex > iter._partIndex)
        return false;

    return _index <= iter._index;
}

const geos::geom::Coordinate &VertexIterator::operator*() const
{
    if ( _linearPosition >= 0 && _linearPosition < _linearSize)
        if ( _pointMode){
            return *_pointCoordinates[_index];
        }
        return _coordinates[_partIndex]->getAt(_index);
    throw ErrorObject(TR("vertex index out of range"));
}

geos::geom::Coordinate &VertexIterator::operator*()
{
    if ( _linearPosition >= 0 && _linearPosition < _linearSize)
        if ( _pointMode){
            return *const_cast<geos::geom::Coordinate *>(_pointCoordinates[_index]);
        }
        return const_cast<geos::geom::Coordinate&>(_coordinates[_partIndex]->getAt(_index));
    throw ErrorObject(TR("vertex index out of range"));
}

geos::geom::Coordinate *VertexIterator::operator->()
{
    if ( _linearPosition >= 0 && _linearPosition < _linearSize)
        if ( _pointMode)
            return const_cast<geos::geom::Coordinate *>(_pointCoordinates[_index]);
        return &const_cast<geos::geom::Coordinate&>(_coordinates[_partIndex]->getAt(_index));
    throw ErrorObject(TR("vertex index out of range"));
}

VertexIterator VertexIterator::operator-(int n)
{
    VertexIterator iter(*this);
    iter -= n;
    return iter;
}

VertexIterator VertexIterator::operator+(int n)
{
    VertexIterator iter(*this);
    iter += n;
    return iter;
}

bool VertexIterator::nextSubGeometry() const
{
    return _nextSubGeometry;
}

void VertexIterator::move(int n)
{
    _index += n;
    _linearPosition += n;
    _nextSubGeometry = false;
    if ( _pointMode){
        _nextSubGeometry = true;
        if ( _index >= _pointCoordinates.size())    {
            _index = _pointCoordinates.size();
        } else if ( _index < 0){
            _index = -1;
        }
    } else {
        if (_index >= _coordinates[_partIndex]->size()){
            ++_partIndex;
            _index = 0;
            if ( _polygonMode){
                _nextSubGeometry =  _polygonMode  ? _partIndex == 0 : false;
            }else
                _nextSubGeometry = true;
            if ( _partIndex >= _coordinates.size()){
                _partIndex = _coordinates.size();
                _linearPosition = _linearSize;
            }
        } else{
            if ( _index < 0){
                --_partIndex;
                if ( _partIndex < 0){
                    _partIndex = 0;
                    _linearPosition = -1;
                    _index = -1;
                }else{
                    _index = _coordinates[_partIndex]->size() - 1;
                }
            }
        }
    }
}

void VertexIterator::setFromGeometry(geos::geom::Geometry *geom)
{
    if ( geom == 0){
        _partIndex = 0;
        _linearPosition = -1;
        _index = -1;
        return ;
    }
    if ( geom->getGeometryTypeId() == geos::geom::GEOS_LINESTRING){
        _coordinates.resize(1);
        storeLineString(dynamic_cast<geos::geom::LineString *>(geom), 0);

    } else if ( geom->getGeometryTypeId() == geos::geom::GEOS_MULTILINESTRING) {
        _coordinates.resize(geom->getNumGeometries());
        for(int j =0; j < geom->getNumGeometries(); ++j){
            storeLineString(dynamic_cast<const geos::geom::LineString *>(geom->getGeometryN(j)), j);
        }
    } else if ( geom->getGeometryTypeId() == geos::geom::GEOS_POINT){
        geos::geom::Point *pnt = dynamic_cast<geos::geom::Point *>(geom);
        _pointCoordinates.resize(1);
        _pointCoordinates[0] = pnt->getCoordinate();
        _linearSize += 1;
        _pointMode = true;
    } else if ( geom->getGeometryTypeId() == geos::geom::GEOS_MULTIPOINT) {
        _coordinates.resize(geom->getNumGeometries());
        for(int j =0; j < geom->getNumGeometries(); ++j){
            const geos::geom::Point *pnt = dynamic_cast<const geos::geom::Point *>(geom->getGeometryN(j));
            _pointCoordinates.resize(j);
            _pointCoordinates[j] = pnt->getCoordinate();
            _linearSize += _coordinates.size();
        }
        _pointMode = true;
    }else if ( geom->getGeometryTypeId() == geos::geom::GEOS_POLYGON){
        geos::geom::Polygon *pol = dynamic_cast<geos::geom::Polygon *>(geom);
         _coordinates.resize(1 + pol->getNumInteriorRing());
        storeLineString(pol->getExteriorRing(),0);
        for(int j=0; j <pol->getNumInteriorRing(); ++j ){
            storeLineString(pol->getInteriorRingN(j),j);
        }
        _polygonMode = true;

    } else if ( geom->getGeometryTypeId() == geos::geom::GEOS_MULTIPOLYGON){
        //TODO most likely this isnt yet complete, dont have a multipolygon test file atm
        _coordinates.resize(0);
        for(int j =0; j < geom->getNumGeometries(); ++j){
            int baseIndex = _coordinates.size();
            const geos::geom::Polygon *pol = dynamic_cast<const geos::geom::Polygon *>(geom->getGeometryN(j));
            _coordinates.resize(_coordinates.size() + 1 + pol->getNumInteriorRing());
            storeLineString(pol->getExteriorRing(),0);
            for(int j=0; j <pol->getNumInteriorRing(); ++j ){
                storeLineString(pol->getInteriorRingN(j),baseIndex + j + 1);
            }
        }
        _polygonMode = true;

    }
}

bool VertexIterator::compatible(const VertexIterator &iter) const
{
    if ( _coordinates.size() != iter._coordinates.size())
        return false;
    if ( _linearSize != iter._linearSize)
        return false;

    int   j= 0;
    for(auto vec : _coordinates){
        if ( vec->size() != iter._coordinates[j]->size())
            return false;
    }
    return true;
}

