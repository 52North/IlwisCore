#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <boost/variant.hpp>
#include "boostext.h"

namespace Ilwis {

class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;

typedef boost::variant<Pixel, Coordinate2d, Coordinate, Line2D<Coordinate2d>, Line2D<Pixel>, Polygon > GeometryType;

class KERNELSHARED_EXPORT Geometry {
public:
    Geometry() {}
    Geometry(const GeometryType& geom) ;

    template<typename GeometryType> GeometryType toType() const{
        return boost::get<GeometryType>(_geometry);
    }

    template<typename GeometryType> void setGeometry(const GeometryType& geom) {
        _geometry = geom;
    }

    bool isValid() const ;
    Box2D<double> envelope() ;
    Box2D<double> envelope() const;

    IlwisTypes ilwisType() const ;
    Geometry transform(const ICoordinateSystem& csySource,const ICoordinateSystem& csyTarger) const;

    bool within(const Geometry& geom) const;


private:
    GeometryType _geometry;
    Box2D<double> _bounds;
};
}

#endif // GEOMETRY_H
