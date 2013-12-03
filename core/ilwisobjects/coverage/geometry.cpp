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
    //TODO
    return true;
}

Box2D<double> Geometry::envelope() const{
    if (!_bounds.isValid()) {
        return const_cast<Geometry *>(this)->envelope();
    }
    return _bounds;
}

bool Geometry::fromWKT(const QString &wkt){
    //        typedef model::ring< point > ring;
    //        typedef model::polygon< point > polygon;
    //        typedef model::multi_polygon< polygon > polygons;
    //        typedef model::box< point > box;
    if (!wkt.left(5).compare("POINT",Qt::CaseInsensitive)){
        if (wkt.left(8).right(3).contains("Z",Qt::CaseInsensitive)){
//            boost::geometry::coordinate_system<> csy;
//            typedef boost::geometry::model::point<double,3,csy> point;
//            boost::geometry::read_wkt(wkt,p);
//            std::cout << boost::geometry::dsv(p) << std::endl;
        }

    }

    if (!wkt.left(7).compare("POLYGON",Qt::CaseInsensitive)){
        Polygon poly;
        boost::geometry::read_wkt(wkt.toStdString(), poly);
        boost::geometry::correct(poly);
        _geometry = poly;
    }
    return true;
}

QString Geometry::toWKT() const{
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
                    ret << boost::geometry::wkt(boost::get<Line2D<Coordinate2d> >(_geometry));
                    break;
                }
                case 4:{
                    //TODO create line3d
                    ret << boost::geometry::wkt(boost::get<Line2D<Coordinate2d> >(_geometry));
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
                return Geometry(pnt);
            }
            case 2:{
                Point3D<double> p = (boost::get<Point3D<double> >(_geometry));
                Point3D<double> pnt(csyTarget->coord2coord(_csy, Coordinate2d(p.x(), p.y()))) ;
                return Geometry(pnt, csyTarget);
            }
            case 3:
            case 4:{
                const Line2D<Coordinate2d>& line = (boost::get<Line2D<Coordinate2d> >(_geometry));
                Line2D<Coordinate2d > newline(line.size());
                std::transform(line.begin(), line.end(),newline.begin(),[&](const Coordinate2d& crd) -> Coordinate2d
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
