#include <QStringList>
#include <boost/geometry.hpp>
#include "ilwis.h"
#include "geometries.h"
#include "geometry.h"

using namespace Ilwis;

Geometry::Geometry(const GeometryType& geom) : _geometry(geom){

}

bool Geometry::isValid() const {
    //TODO
    return true;
}

Box3D<double> Geometry::envelope() {
    if (!_bounds.isValid()) {
        switch(_geometry.which()){
        case 0:
        {
            Point2D<double> p = (boost::get<Point2D<double> >(_geometry));
            _bounds = Box3D<double>(p,p);
            break;
        }
        case 1:
        {
            Point3D<double> p = (boost::get<Point3D<double> >(_geometry));
            _bounds =  Box3D<double>(p,p);
            break;
        }
        case 2:
        {
            const Line2D<Coordinate2d>& line = (boost::get<Line2D<Coordinate2d> >(_geometry));
            _bounds = boost::geometry::return_envelope<Box2D<double> >(line);

        }
        case 3:
        case 4:
        case 5:
            return Box3D<double>();
        }
    }
    return _bounds;
}

IlwisTypes Geometry::ilwisType() const {
    switch(_geometry.which()){
    case 0:
    case 1:
    case 2:
        return itPOINTCOVERAGE;
    case 3:
    case 4:
        return itSEGMENTCOVERAGE;
    case 5:
    case 6:
        return itPOLYGONCOVERAGE;
    }
    return itUNKNOWN;
}
