#include <QStringList>
#include "kernel.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "line.h"
#include <type_traits>
#include "polygon.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "geometry.h"
#include "boost/geometry/core/coordinate_system.hpp"

using namespace Ilwis;

Geometry::Geometry(const GeometryType& geom, const ICoordinateSystem &csy) :
    _geometry(geom),
    _bounds(Coordinate2d(rUNDEF, rUNDEF), Coordinate2d(rUNDEF, rUNDEF)),
    _csy(csy)
{

}

Geometry::Geometry(const QString &wkt, const ICoordinateSystem &csy):
    _bounds(Coordinate2d(rUNDEF, rUNDEF), Coordinate2d(rUNDEF, rUNDEF)),
    _csy(csy)
{
    if (!this->fromWKT(wkt))
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Geometry",wkt);
}

bool Geometry::isValid() const {
    //TODO:
    return true;
}

Box2D<double> Geometry::envelope() const{
    if (!_bounds.isValid()) {
        return const_cast<Geometry *>(this)->envelope();
    }
    return _bounds;
}

bool Geometry::fromWKT(const QString &wkt){
    int dim = wktDimensions(wkt);
    if ( wkt.indexOf("POLYGON", 0, Qt::CaseInsensitive) == 0) {
        if (dim == 2){
            Polygon polygon;
            boost::geometry::read_wkt(wkt.toStdString(), polygon);
            boost::geometry::correct(polygon);
            _geometry = polygon;
        }else
            return false;
    }else if ( wkt.indexOf("LINESTRING", 0, Qt::CaseInsensitive) == 0) {
        if ( dim == 2){
            Line2D line;
            boost::geometry::read_wkt(wkt.toStdString(), line);
            boost::geometry::correct(line);
            _geometry = line;
        }else{
            Line3D line;
            boost::geometry::read_wkt(wkt.toStdString(), line);
            boost::geometry::correct(line);
            _geometry = line;
            }
    } else if ( wkt.indexOf("POINT", 0, Qt::CaseInsensitive) == 0) {
        if ( dim == 2){
            Coordinate2d point;
            boost::geometry::read_wkt(wkt.toStdString(), point);
            boost::geometry::correct(point);
            _geometry = point;
        } else if ( dim == 3) {
            Coordinate point;
            boost::geometry::read_wkt(wkt.toStdString(), point);
            boost::geometry::correct(point);
            _geometry = point;
        }else
            return false;
    }else
        return false;
    return true;
}

const QString Geometry::toWKT() const{
    if (!_bounds.isValid()) {
        std::stringstream ret;
        try{
            switch(_geometry.which()){
                case 0:{
                    ret << boost::geometry::wkt(boost::get<Point2D<> >(_geometry));
                    break;
                }
                case 1:{
                    ret << boost::geometry::wkt(boost::get<Point2D<double> >(_geometry));
                    break;
                }
                case 2:{
                    ret << boost::geometry::wkt(boost::get<Point3D<double> >(_geometry));
                    break;
                }
                case 3:{
                    ret << boost::geometry::wkt(boost::get<Line2D >(_geometry));
                    break;
                }
                case 4:{
                    ret << boost::geometry::wkt(boost::get<Line3D >(_geometry));
                    break;
                }
                case 5:{
                    ret << boost::geometry::wkt(boost::get<Polygon >(_geometry));
                    break;
                }
                default:
                    return QString("EMPTY");
            }
        }catch (boost::bad_get bg){
            ERROR2(ERR_COULD_NOT_CONVERT_2,"boost::geometry",QString("WKT - %1").arg(bg.what()));
            return QString();
        }
        return QString(ret.str().c_str());
    }else{
        ERROR2(ERR_COULD_NOT_CONVERT_2,"boost::geometry",QString("WKT - invalid geomtry"));
        return QString();
    }
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
            const Line2D& line = (boost::get<Line2D >(_geometry));
            _bounds = boost::geometry::return_envelope<Box2Dd>(line);
            break;
        }
        case 4:
        {
            //TODO: create line3d
            const Line3D& line = (boost::get<Line3D >(_geometry));
            _bounds = boost::geometry::return_envelope<Box3D<double> >(line);
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

IlwisTypes Geometry::geometryType() const {
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

Geometry Geometry::transform(const ICoordinateSystem &csyTarget) const
{
    if ( !csyTarget.isValid()|| !_csy.isValid() || _csy == csyTarget){
        return *this;
    }
    try{
        switch(_geometry.which()){
            case 0:
            case 1:{
                Point2D<double> p = (boost::get<Point2D<double> >(_geometry));
                Point2D<double> pnt(csyTarget->coord2coord(_csy, Coordinate2d(p.x(), p.y()))) ;
        	return Geometry(pnt, csyTarget);
            }
            case 2:{
                Point3D<double> p = (boost::get<Point3D<double> >(_geometry));
                Point3D<double> pnt(csyTarget->coord2coord(_csy, Coordinate2d(p.x(), p.y()))) ;
                return Geometry(pnt, csyTarget);
            }
            case 3:{
                const Line2D& line = (boost::get<Line2D>(_geometry));
                Line2D newline(line.size());
                std::transform(line.begin(), line.end(),newline.begin(),[&](const Coordinate2d& crd) -> Coordinate2d
                    { return csyTarget->coord2coord(_csy, crd);});
                return Geometry(newline, csyTarget);
            }
            case 4:{
                const Line3D& line = (boost::get<Line3D >(_geometry));
                Line3D newline(line.size());
                std::transform(line.begin(), line.end(),newline.begin(),[&](const Coordinate& crd) -> Coordinate
                    { return csyTarget->coord2coord(_csy, crd);});
                return Geometry(newline, csyTarget);
            }
            case 5:
            case 6:{
                const Polygon& pol = (boost::get<Polygon >(_geometry));
                Polygon newPol;
                newPol.outer().resize(pol.outer().size());
                newPol.inners().resize(pol.inners().size());
                std::transform(pol.outer().begin(), pol.outer().end(),newPol.outer().begin(),[&](const Coordinate2d& crd) -> Coordinate2d
                    {return csyTarget->coord2coord(_csy, crd);});
                auto iter = newPol.inners().begin();
                for(const auto& hole : pol.inners()){
                    (*iter).resize(hole.size());
                    std::transform(hole.begin(), hole.end(),(*iter).begin(),[&](const Coordinate2d& crd) -> Coordinate2d
                        { return csyTarget->coord2coord(_csy, crd);});
                    ++iter;

                }
                return Geometry(newPol, csyTarget);
            }
        }
    } catch(boost::bad_get& ex) {
        kernel()->issues()->log(ex.what());
    }

    return Geometry();
}

ICoordinateSystem Geometry::coordinateSystem() const{
    return _csy;
}
void Geometry::coordinateSystem(const ICoordinateSystem& csy){
    _csy = csy;
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


int Geometry::wktDimensions(const QString &wkt){
    if(-1 != wkt.indexOf(" ZM",5,Qt::CaseInsensitive)){
        return 4;
    }else if ((-1 != wkt.indexOf(" Z",5,Qt::CaseInsensitive)) ||
              (-1 != wkt.indexOf(" M",5,Qt::CaseInsensitive))
              ){
        return 3;
    }else{
        if ((0 == wkt.indexOf("POINT",0,Qt::CaseInsensitive)) &&
             wkt.left(wkt.lastIndexOf(")")).mid(wkt.indexOf("(")+1).split(QRegExp("\\s+"),QString::SkipEmptyParts).count() == 3
            ){
            return 3;
        }//TODO 3D/4D LINESTING and POLYGON dimension parsing without Z/ZM/M ?
        return 2;
    }
}
