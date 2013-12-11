#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <boost/variant.hpp>
#include "boostext.h"
#include <QString>

namespace Ilwis {

class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;

//WARNING: add new types to the back else the switch/case statements in the class have to be adapted
typedef boost::variant<Pixel, Coordinate2d, Coordinate, Line2D, Line3D, Polygon > GeometryType;


class KERNELSHARED_EXPORT Geometry {
public:
    Geometry() {}
    Geometry(const GeometryType& geom, const ICoordinateSystem& csy) ;
    Geometry(const QString &wkt, const ICoordinateSystem& csy = ICoordinateSystem()) ;

    template<typename GeometryType> GeometryType toType() const{
        return boost::get<GeometryType>(_geometry);
    }

    template<typename GeometryType> void setGeometry(const GeometryType& geom) {
        _geometry = geom;
    }

    ICoordinateSystem coordinateSystem() const;
    void coordinateSystem(const ICoordinateSystem&);

    bool isValid() const ;
    Box2D<double> envelope() ;
    Box2D<double> envelope() const;

    bool fromWKT(const QString &wkt);
    const QString toWKT() const;

    IlwisTypes geometryType() const ;
    Geometry transform(const ICoordinateSystem& csyTarger) const;

    bool within(const Geometry& geom) const;


private:
    GeometryType _geometry;
    Box2D<double> _bounds;
    ICoordinateSystem _csy;
    int wktDimensions(const QString &wkt);
};
}

#endif // GEOMETRY_H
