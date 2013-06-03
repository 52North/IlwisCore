#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <boost/variant.hpp>

namespace Ilwis {

typedef boost::variant<Pixel, Coordinate2d, Coordinate, Line2D<Coordinate2d>, Line2D<Coordinate>, Line2D<Pixel>, Polygon > GeometryType;

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
    Box3D<double> envelope() ;
    IlwisTypes ilwisType() const ;


private:
    GeometryType _geometry;
    Box3D<double> _bounds;
};
}

#endif // GEOMETRY_H
