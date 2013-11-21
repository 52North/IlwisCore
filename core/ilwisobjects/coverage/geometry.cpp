#include <QStringList>
#include "kernel.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "line.h"
#include <type_traits>
#include <boost/geometry.hpp>
#include "polygon.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "geometry.h"

using namespace Ilwis;

Geometry::Geometry(const GeometryType& geom) : _geometry(geom), _bounds(Coordinate2d(rUNDEF, rUNDEF), Coordinate2d(rUNDEF, rUNDEF)){

}

bool Geometry::isValid() const {
    //TODO
    return true;
}

Box2D<double> Geometry::envelope() const{
    if (!_bounds.isValid()) {
        return const_cast<Geometry *>(this)->envelope();
    }
    return _bounds;
}

Box2D<double> Geometry::envelope() {
    if (!_bounds.isValid()) {
        switch(_geometry.which()){
        case 0:
        {
            Point2D<double> p = (boost::get<Point2D<> >(_geometry));
            _bounds = Box2D<double>(p,p);
            break;
        }
        case 1:
        {
            Point3D<double> p = (boost::get<Point2D<double> >(_geometry));
            _bounds =  Box2D<double>(p,p);
            break;
        }
        case 2:
        {
            Point3D<double> p = (boost::get<Point3D<double> >(_geometry));
            _bounds =  Box2D<double>(p,p);
            break;
        }
        case 3:
        {
            const Line2D<Coordinate2d>& line = (boost::get<Line2D<Coordinate2d> >(_geometry));
            _bounds = boost::geometry::return_envelope<Box2Dd>(line);
            break;
        }
        case 4:
        {
            //TODO create line3d
            const Line2D<Coordinate2d>& line = (boost::get<Line2D<Coordinate2d> >(_geometry));
            _bounds = boost::geometry::return_envelope<Box2D<double> >(line);
            break;
        }
        case 5:
        {
            Polygon& pol = (boost::get<Polygon >(_geometry));
            _bounds = boost::geometry::return_envelope<Box2D<double> >(pol);
        }

        case 6:
        case 7:
            return Box2D<double>();
        }
    }
    return _bounds;
}

IlwisTypes Geometry::ilwisType() const {
    switch(_geometry.which()){
    case 0:
    case 1:
    case 2:
        return itPOINT;
    case 3:
    case 4:
        return itLINE;
    case 5:
    case 6:
        return itPOLYGON;
    }
    return itUNKNOWN;
}

Geometry Geometry::transform(const ICoordinateSystem &csySource, const ICoordinateSystem &csyTarget) const
{
    if ( !csyTarget.isValid() || csySource == csyTarget){
        return *this;
    }
    try{
    switch(_geometry.which()){
    case 0:
    case 1:{
        Point2D<double> p = (boost::get<Point2D<double> >(_geometry));
        Point2D<double> pnt(csyTarget->coord2coord(csySource, Coordinate2d(p.x(), p.y()))) ;
        return Geometry(pnt);
    }
    case 2:{
        Point3D<double> p = (boost::get<Point3D<double> >(_geometry));
        Point3D<double> pnt(csyTarget->coord2coord(csySource, Coordinate2d(p.x(), p.y()))) ;
        return Geometry(pnt);
    }
    case 3:
    case 4:{
        const Line2D<Coordinate2d>& line = (boost::get<Line2D<Coordinate2d> >(_geometry));
        Line2D<Coordinate2d > newline(line.size());
        std::transform(line.begin(), line.end(),newline.begin(),[&](const Coordinate2d& crd) -> Coordinate2d
            { return csyTarget->coord2coord(csySource, crd);});
        return Geometry(newline);
    }
    case 5:
    case 6:{
        const Polygon& pol = (boost::get<Polygon >(_geometry));
        Polygon newPol;
        newPol.outer().resize(pol.outer().size());
        newPol.inners().resize(pol.inners().size());
        std::transform(pol.outer().begin(), pol.outer().end(),newPol.outer().begin(),[&](const Coordinate2d& crd) -> Coordinate2d
            {return csyTarget->coord2coord(csySource, crd);});
        auto iter = newPol.inners().begin();
        for(const auto& hole : pol.inners()){
            (*iter).resize(hole.size());
            std::transform(hole.begin(), hole.end(),(*iter).begin(),[&](const Coordinate2d& crd) -> Coordinate2d
                { return csyTarget->coord2coord(csySource, crd);});
            ++iter;

        }
        return Geometry(newPol);
    }
    }
    } catch(boost::bad_get& ex) {
        kernel()->issues()->log(ex.what());
    }

    return Geometry();
}

bool Geometry::within(const Geometry &geom) const{
    //until now only for point.within(polygon)
    if ( geom._geometry.which() == 5 && _geometry.which() >= 0 && _geometry.which() <= 2) {
        const Polygon& pol = (boost::get<Polygon >(geom._geometry));
        Point2D<double> p;
        if (_geometry.which() == 0)
            p = (boost::get<Pixel >(_geometry));
        else if (_geometry.which() == 1)
            p = (boost::get<Coordinate2d >(_geometry));
        else if (_geometry.which() == 2)
            p = (boost::get<Coordinate >(_geometry));
        return boost::geometry::within(p, pol);
    }
    return false;
}
