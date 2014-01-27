#include <QStringList>
#include "kernel.h"
#include "geometries.h"
#include <type_traits>
#include "polygon.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "geometry.h"
#include "geometryoperations.h"

using namespace Ilwis;

Within::Within()
{
}

GeometryMethod *Within::create() const{
        return new Within();
}

bool Within::execute(bool& outBool, const Geometry& container, const Geometry& containee) const{
    //until now only for point.within(polygon)
    outBool = false;
    if ( container.geometryType() == itPOLYGON && containee.geometryType() == itPOINT) {
        const Polygon& pol = container.toType<Polygon>();
        Point2D<double> p;
        const GeometryType& point = containee.geometry();
        if (point.which() == 0)
            p = containee.toType<Pixel>();
        else if (point.which() == 1)
            p = containee.toType<Coordinate2d >();
        else if (point.which() == 2)
            p = containee.toType<Coordinate>();
        outBool =  boost::geometry::within(p, pol);
    }
    return false;
}

//--------------------------------------------------------
Area::Area()
{
}

GeometryMethod *Area::create() const{
        return new Area();
}

bool Area::execute(double& outval, const Geometry& geom, const Geometry&) const{
    return false;
}
//----------------------------------------------------------
Centroid::Centroid()
{
}

GeometryMethod *Centroid::create() const{
        return new Centroid();
}

bool Centroid::execute(Geometry& outGeom, const Geometry& geom1, const Geometry &) const{
    return false;
}

//--------------------------------------------------------
ConvexHull::ConvexHull()
{
}

GeometryMethod *ConvexHull::create() const{
        return new Area();
}

bool ConvexHull::execute(Geometry& outGeom, const Geometry& geom, const Geometry &) const{
    return false;
}
