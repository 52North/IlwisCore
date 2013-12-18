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

/**
 * The Geometry class represents the geometry of a feature. a feature can thus have the following Geometries: <br>
 * Pixel, Coordinate2d, Coordinate, Line2D, Line3D or Polygon. There are no multi geometries implemented at this moment <br>
 * because multi geometries should rather be implemented by
 */
class KERNELSHARED_EXPORT Geometry {
public:

    /**
     * @brief Geometry
     */
    Geometry() {}

    /**
     * @brief Geometry
     * @param geom
     * @param csy
     */
    Geometry(const GeometryType& geom, const ICoordinateSystem& csy) ;

    /**
     * @brief Geometry
     * @param wkt
     * @param csy
     */
    Geometry(const QString &wkt, const ICoordinateSystem& csy = ICoordinateSystem()) ;

    /**
     *
     */
    template<typename GeometryType> GeometryType toType() const{
        return boost::get<GeometryType>(_geometry);
    }

    /**
     *
     */
    template<typename GeometryType> void setGeometry(const GeometryType& geom) {
        _geometry = geom;
    }

    /**
     * @brief coordinateSystem
     * @return
     */
    ICoordinateSystem coordinateSystem() const;

    /**
     * @brief coordinateSystem
     */
    void coordinateSystem(const ICoordinateSystem&);

    /**
     * @brief isValid
     * @return
     */
    bool isValid() const ;

    /**
     * @brief envelope
     * @return
     */
    Box2D<double> envelope() ;

    /**
     * @brief envelope
     * @return
     */
    Box2D<double> envelope() const;

    /**
     * @brief fromWKT
     * @param wkt
     * @return
     */
    bool fromWKT(const QString &wkt);

    /**
     * @brief toWKT
     * @return
     */
    const QString toWKT() const;

    /**
     * @brief geometryType
     * @return
     */
    IlwisTypes geometryType() const ;

    /**
     * @brief transform
     * @param csyTarger
     * @return
     */
    Geometry transform(const ICoordinateSystem& csyTarger) const;

    /**
     * @brief within
     * @param geom
     * @return
     */
    bool within(const Geometry& geom) const;

private:
    GeometryType _geometry;
    Box2D<double> _bounds;
    ICoordinateSystem _csy;
    int wktDimensions(const QString &wkt);
};
}

#endif // GEOMETRY_H
