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
     * constructs an empty Geometry
     */
    Geometry() {}

    /**
     * Copy constructor for a Geometry on a given coordinatesystem
     *
     * @param geom the Geometry that should be copied
     * @param csy The coordinateSystem the geometry should be copied onto
     */
    Geometry(const GeometryType& geom, const ICoordinateSystem& csy) ;

    /**
     * Constructs a new Geometry from a String on a coordinatesystem.<br>
     * The string must be constructed to the wkt string standard.
     *
     * @param wkt The String, in wkt standard
     * @param csy The target coordinates system, defaults to a new empty ICoordinateSystem
     */
    Geometry(const QString &wkt, const ICoordinateSystem& csy = ICoordinateSystem()) ;

    /**
     *
     */
    template<typename GeometryType> GeometryType toType() const{
        return boost::get<GeometryType>(_geometry);
    }

    /**
     * template function. sets the type of this geometry. can be:
     * - Pixel
     * - Coordinate2d
     * - Coordinate
     * - Line2D
     * - Line3D
     * - Polygon
     */
    template<typename GeometryType> void setGeometry(const GeometryType& geom) {
        _geometry = geom;
    }

    /**
     * Query of the ICoordinateSystem of this geometry
     *
     * @return The CoordinateSystem of this geometry
     */
    ICoordinateSystem coordinateSystem() const;

    /**
     * Sets the CoordinateSystem of this geometry
     */
    void coordinateSystem(const ICoordinateSystem&);

    //@override
    bool isValid() const ;

    /**
     * Query for the bounding box of this geometry.<br>
     * The geometry will always fit in the boundingbox.
     *
     * @return The bounding box in a Box2D<double> format
     */
    Box2D<double> envelope() ;

    /**
     * Query for the bounding box of this geometry.<br>
     * The geometry will always fit in the boundingbox.
     *
     * @return The bounding box in a Box2D<double> format
     */
    Box2D<double> envelope() const;

    /**
     * Generates this (empty) geometry using the supplied wkt string. if this geometry was already valid it will override the excesting values
     *
     * @param wkt the wkt string with the new geometry
     * @return true if the string was read succesful
     */
    bool fromWKT(const QString &wkt);

    /**
     * Generates a wkt string that represents this geometry
     *
     * @return A String in wkt format
     */
    const QString toWKT() const;

    /**
     * Query for the ilwis type of the geometry<br>
     * results may be:
     * - Pixel
     * - Coordinate2d
     * - Coordinate
     * - Line2D
     * - Line3D
     * - Polygon
     *
     * \sa IlwisObject
     * @return the IlwisType of this geometry
     */
    IlwisTypes geometryType() const ;

    /**
     * Generates a Geometry copy of this geometry on the given target coordinate system<br>
     * both coordinate systems must not be the same and the target must be valid.
     *
     * @param csyTarger the target coordinatesystem
     * @return the new geometry with as coordinatesystem the target
     */
    Geometry transform(const ICoordinateSystem& csyTarger) const;

    /**
     * //Checks if a certain geometry is inside (within) another geometry//
     * Checks if a point geometry is inside a polygon geometry, other cases not yet implemented
     *
     * @param geom the geometry that has to be within
     * @return true if the geometry fits in this
     */
    bool within(const Geometry& geom) const;

private:
    GeometryType _geometry;
    Box2D<double> _bounds;
    ICoordinateSystem _csy;
    bool _valid;
    int wktDimensions(const QString &wkt);
};
}

#endif // GEOMETRY_H
