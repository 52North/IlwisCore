#ifndef POLYGON_H
#define POLYGON_H

#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
#include <boost/geometry/geometries/polygon.hpp>

namespace Ilwis {
typedef boost::geometry::model::polygon<Coordinate2d> Polygon;
}

#endif // POLYGON_H
