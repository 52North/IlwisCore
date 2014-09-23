#include "geos/geom/Geometry.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/LineString.h"
#include "geos/geom/Point.h"
#include "geos/geom/Polygon.h"
#include "geos/io/ParseException.h"
#include "coverage.h"
#include "errorobject.h"
#include "featurecoverage.h"
#include "feature.h"
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
    geos::geom::Geometry *geom = GeometryHelper::fromWKT(wkt, ICoordinateSystem());
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
    _polygonMode = iter._polygonMode;
    _hasOwnership = iter._hasOwnership;
    _linearSize = iter._linearSize;
    _linearPosition = iter._linearPosition;
    if ( iter._internalGeom)
        _internalGeom.reset(iter._internalGeom->clone());
    else
        _internalGeom.reset(0);

    return *this;
}

void VertexIterator::storeLineString(const geos::geom::LineString *cline, int index, bool isInterior){
    const geos::geom::CoordinateSequence *seq = cline->getCoordinatesRO();
     _coordinates[index] = {seq, isInterior};
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
        if ( _pointMode){
            return *const_cast<geos::geom::Coordinate*>(_pointCoordinates[n]);
        }
        else{
            int j = 0;
            for(auto& vec : _coordinates){
                if ( n < vec._crds->size()){
                    return const_cast<geos::geom::Coordinate&>(_coordinates[j]._crds->getAt(n));
                }
                n-= vec._crds->size();
                ++j;
            }
        }
    }
    throw ErrorObject(TR("vertex index out of range"));
}

bool VertexIterator::isAtEnd(const VertexIterator &iter) const {
    return iter._coordinates.size() == iter._partIndex &&  iter._index == 0;
}

bool VertexIterator::operator==(const VertexIterator &iter) const
{
    if ( _index != iter._index)
        return false;
    if ( _partIndex !=  iter._partIndex)
        return false;
    if (!compatible(iter))
        return false;
    if(_pointMode){
        if(_index == _pointCoordinates.size() - 1){
            return true;
        }
    } else {
        if ( isAtEnd(iter) && isAtEnd(*this))
            return true;
    }
    int j = 0;
    if ( _pointMode){
         for(int i=0; i < _pointCoordinates.size(); ++i){
             if(!(_pointCoordinates[i]->equals(*iter._pointCoordinates[i])))
                 return false;
         }
         return true;
    }
    for(const auto& vec : _coordinates){

        for( int  i = 0; i <  vec._crds->size(); ++i){
            if ( vec._crds->getAt(i) != iter._coordinates[j]._crds->getAt(i))
                return false;
        }

        ++j;
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
    if ( _partIndex < iter._partIndex)
        return true;
    return _index < iter._index;
}

bool VertexIterator::operator>(const VertexIterator &iter) const
{
    if(!compatible(iter))
        return false;
    if ( _partIndex < iter._partIndex)
        return false;
    if ( _partIndex > iter._partIndex)
        return true;
    return _index > iter._index;
}

bool VertexIterator::operator<=(const VertexIterator &iter) const
{
    if(!compatible(iter))
        return false;
    if ( _partIndex > iter._partIndex)
        return false;
    return _index <= iter._index;
}

bool VertexIterator::operator>=(const VertexIterator &iter) const
{
    if(!compatible(iter))
        return false;
    if ( _partIndex < iter._partIndex)
        return false;

    return _index >= iter._index;
}

const geos::geom::Coordinate &VertexIterator::operator*() const
{
    if ( _linearPosition >= 0 && _linearPosition < _linearSize)
        if ( _pointMode){
            return *_pointCoordinates[_index];
        }
        return _coordinates[_partIndex]._crds->getAt(_index);
    throw ErrorObject(TR("vertex index out of range"));
}

geos::geom::Coordinate &VertexIterator::operator*()
{
    if ( (_linearPosition >= 0 && _linearPosition < _linearSize) && _partIndex >= 0 &&  _partIndex < _coordinates.size() ){
        if ( _pointMode){
            return *const_cast<geos::geom::Coordinate *>(_pointCoordinates[_index]);
        }
        return const_cast<geos::geom::Coordinate&>(_coordinates[_partIndex]._crds->getAt(_index));
    }
    throw ErrorObject(TR("vertex index out of range"));
}

geos::geom::Coordinate *VertexIterator::operator->()
{
    if ( _linearPosition >= 0 && _linearPosition < _linearSize)
        if ( _pointMode)
            return const_cast<geos::geom::Coordinate *>(_pointCoordinates[_index]);
        return &const_cast<geos::geom::Coordinate&>(_coordinates[_partIndex]._crds->getAt(_index));
    throw ErrorObject(TR("vertex index out of range"));
}

VertexIterator VertexIterator::operator-(int n) const
{
    VertexIterator iter(*this);
    iter -= n;
    return iter;
}

VertexIterator VertexIterator::operator+(int n) const
{
    VertexIterator iter(*this);
    iter += n;
    return iter;
}

bool VertexIterator::nextSubGeometry() const
{
    return _nextSubGeometry;
}

bool VertexIterator::isInteriorVertex() const{
    if ( _polygonMode)
        return _coordinates[_partIndex]._isInterior;
    return false;
}
void VertexIterator::move(int n)
{
    if (_coordinates.size() == 0){
        _index = ENDVERTEX;
        return;
    }

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
        if ( _coordinates.size() <= _partIndex){
            _partIndex = _coordinates.size(); // endcondition
            _index = 0;
            _linearPosition = _linearSize; // size is always position+1, so linearsize is one beyound the last position if used as position
            return;
        }

        int sz =  _coordinates[_partIndex]._crds->size();
        if (_index >=sz){ // weird compiler bug; (mingw 4.8.2), using result directly always leads to positive test
            ++_partIndex;
            _index = 0;
            _nextSubGeometry = true;
            if ( n == ENDVERTEX){
                _partIndex = _coordinates.size();
                _index = 0;
                _linearPosition = _linearSize; // size is always position+1, so linearsize is one beyound the last position if used as position
            }
        } else{
            if ( _index < 0){
                --_partIndex;
                if ( _partIndex < 0){
                    _partIndex = 0;
                    _linearPosition = 0;
                    _index = 0;
                }else{
                    _index = _coordinates[_partIndex]._crds->size() - 1;
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
        _coordinates.resize(1);
        _pointCoordinates.resize(1);
        _pointCoordinates[0] = pnt->getCoordinate();
        _linearSize += 1;
        _pointMode = true;
    } else if ( geom->getGeometryTypeId() == geos::geom::GEOS_MULTIPOINT) {
        _coordinates.resize(geom->getNumGeometries());
        for(int j =0; j < geom->getNumGeometries(); ++j){
            const geos::geom::Point *pnt = dynamic_cast<const geos::geom::Point *>(geom->getGeometryN(j));
            _pointCoordinates.resize(j+1);
            _pointCoordinates[j] = pnt->getCoordinate();
            _linearSize += 1;
        }
        _pointMode = true;
    }else if ( geom->getGeometryTypeId() == geos::geom::GEOS_POLYGON){
        geos::geom::Polygon *pol = dynamic_cast<geos::geom::Polygon *>(geom);
         _coordinates.resize(1 + pol->getNumInteriorRing());
        storeLineString(pol->getExteriorRing(),0);
        for(int j=0; j <pol->getNumInteriorRing(); ++j ){
            storeLineString(pol->getInteriorRingN(j),j + 1, true);
        }
        _polygonMode = true;

    } else if ( geom->getGeometryTypeId() == geos::geom::GEOS_MULTIPOLYGON){
        _coordinates.resize(0);
        for(int j =0; j < geom->getNumGeometries(); ++j){
            int baseIndex = _coordinates.size();
            const geos::geom::Polygon *pol = dynamic_cast<const geos::geom::Polygon *>(geom->getGeometryN(j));
            _coordinates.resize(_coordinates.size() + 1 + pol->getNumInteriorRing());
            storeLineString(pol->getExteriorRing(), baseIndex );
            for(int j=0; j <pol->getNumInteriorRing(); ++j ){
                storeLineString(pol->getInteriorRingN(j),baseIndex + j + 1, true);
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
    if(_pointMode){
       return true;
    } else{
        for(auto vec : _coordinates){
            if ( j >= iter._coordinates.size() )
                return false;
            if ( vec._crds->size() != iter._coordinates[j]._crds->size())
                return false;
            ++j;
        }
    }
    return true;
}

