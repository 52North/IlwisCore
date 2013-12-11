#ifndef BOOSTEXT_H
#define BOOSTEXT_H

#include <cstddef>
#include <boost/geometry.hpp>
#include <boost/mpl/int.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/register/point.hpp>

#include <boost/geometry/io/wkt/wkt.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/register/box.hpp>

#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
#include <boost/geometry/geometries/polygon.hpp>

BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(Coordinate2d, double, cs::cartesian, x, y, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(Pixel, qint32, cs::cartesian, x, y, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(Pixel_u, quint32, cs::cartesian, x, y, x, y)
BOOST_GEOMETRY_REGISTER_POINT_3D_GET_SET(Coordinate, double, cs::cartesian, x, y, z, x, y, z)
BOOST_GEOMETRY_REGISTER_POINT_3D_GET_SET(Voxel, qint32, cs::cartesian, x, y, z, x, y, z)
BOOST_GEOMETRY_REGISTER_POINT_3D_GET_SET(Voxel_u, quint32, cs::cartesian, x, y, z, x, y, z)

BOOST_GEOMETRY_REGISTER_LINESTRING(Ilwis::Line2D)
BOOST_GEOMETRY_REGISTER_LINESTRING(Ilwis::Line3D)

BOOST_GEOMETRY_REGISTER_BOX(Box2Dd,Coordinate2d, min_corner(), max_corner())
BOOST_GEOMETRY_REGISTER_BOX(Box2Di,Pixel, min_corner(), max_corner())
BOOST_GEOMETRY_REGISTER_BOX(Box2Dui,Pixel_u, min_corner(), max_corner())

BOOST_GEOMETRY_REGISTER_BOX(Box3Dd,Coordinate, min_corner(), max_corner())
BOOST_GEOMETRY_REGISTER_BOX(Box3Di,Voxel, min_corner(), max_corner())
BOOST_GEOMETRY_REGISTER_BOX(Box3Dui,Voxel_u, min_corner(), max_corner())




#endif // BOOSTEXT_H
