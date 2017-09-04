#include "kernel.h"

#include "geos/geom/Geometry.h"
#include "geos/geom/Coordinate.h"

#include "pythonapi_qtGNUTypedefs.h"

#include "../../core/ilwisobjects/ilwisdata.h"
#include "../../core/util/location.h"
#include "../../core/util/ilwiscoordinate.h"
#include "../../core/util/box.h"
#include "../../core/ilwisobjects/geometry/coordinatesystem/coordinatesystem.h"
#include "../../core/ilwisobjects/coverage/geometryhelper.h"
#include "../../core/ilwisobjects/coverage/vertexiterator.h"

#include "pythonapi_error.h"
#include "pythonapi_geometry.h"
#include "pythonapi_util.h"
#include "pythonapi_vertexiterator.h"


using namespace pythonapi;

VertexIterator::VertexIterator(){
}

VertexIterator::~VertexIterator(){
}

VertexIterator::VertexIterator(const Geometry &geom) : _ilwisVertexIterator(new Ilwis::VertexIterator(geom.ptr())){
}

VertexIterator::VertexIterator(const std::string &wkt){
    QString qWkt = QString::fromStdString(wkt);
    geos::geom::Geometry *geom = Ilwis::GeometryHelper::fromWKT(qWkt, Ilwis::ICoordinateSystem());
    _ilwisVertexIterator.reset(new Ilwis::VertexIterator(geom));
}

VertexIterator::VertexIterator(Ilwis::VertexIterator *ilwIt) : _ilwisVertexIterator(ilwIt){
}

bool VertexIterator::__bool__() const{
    return (bool)this->_ilwisVertexIterator;
}

std::string VertexIterator::__str__() const{
    return "VertexIterator at coordinate: " + current().__str__();
}

VertexIterator* VertexIterator::__iter__(){
    return this;
}

Coordinate VertexIterator::__next__(){
    if(*this != end()){
        Coordinate coord = current();
        this->operator +=(1);
        return coord;
    } else {
        throw StopIteration();
    }
}

VertexIterator VertexIterator::operator +(int n){
    VertexIterator iter(*this);
    iter.ptr() += n;
    return iter;
}

VertexIterator VertexIterator::__radd__(int n){
    VertexIterator iter(*this);
    iter.ptr() += n;
    return iter;
}

VertexIterator* VertexIterator::operator +=(int n){
    this->ptr() += n;
    return this;
}

VertexIterator VertexIterator::operator -(int n){
    VertexIterator iter(*this);
    iter.ptr() -= n;
    return iter;
}

VertexIterator VertexIterator::__rsub__(int n){
    VertexIterator iter(*this);
    iter.ptr() -= n;
    return iter;
}

VertexIterator* VertexIterator::operator -=(int n){
    this->ptr() -= n;
    return this;
}

Coordinate VertexIterator::__getitem__(quint32 linearpos){
    geos::geom::Coordinate geoCoord = this->ptr()[linearpos];
    Ilwis::Coordinate ilwCoord (geoCoord);
    Coordinate* pyCoord = new Coordinate(ilwCoord);
    return *pyCoord;
}

Coordinate VertexIterator::current() const{
    geos::geom::Coordinate geoCoord = *(this->ptr());
    Ilwis::Coordinate ilwCoord (geoCoord);
    Coordinate* pyCoord = new Coordinate(ilwCoord);
    return *pyCoord;
}

bool VertexIterator::operator ==(const VertexIterator& other) const{
    return this->ptr() == other.ptr();
}

bool VertexIterator::operator !=(const VertexIterator& other) const{
    return this->ptr() != other.ptr();
}

bool VertexIterator::operator >=(const VertexIterator& other) const{
    return this->ptr() >= other.ptr();
}

bool VertexIterator::operator <(const VertexIterator& other) const{
    return this->ptr() < other.ptr();
}

bool VertexIterator::operator <=(const VertexIterator& other) const{
    return this->ptr() <= other.ptr();
}

bool VertexIterator::operator >(const VertexIterator& other) const{
    return this->ptr() > other.ptr();
}

bool VertexIterator::nextSubGeometry() const{
    return this->ptr().nextSubGeometry();
}

bool VertexIterator::isInteriorVertex() const{
    return this->ptr().isInteriorVertex();
}

VertexIterator VertexIterator::begin(){
    Ilwis::VertexIterator ilwIt = ::begin(this->ptr());
    return VertexIterator(new Ilwis::VertexIterator(ilwIt));
}

VertexIterator VertexIterator::end (){
    Ilwis::VertexIterator ilwIt = ::end(this->ptr());
    return VertexIterator(new Ilwis::VertexIterator(ilwIt));
}

Ilwis::VertexIterator& VertexIterator::ptr() const{
    if (!this->__bool__())
        throw InvalidObject("use of invalid VertexIterator (ptr)");
    return (*this->_ilwisVertexIterator);
}
