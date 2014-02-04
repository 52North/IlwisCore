#include "Kernel_global.h"
#include "ilwis.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/Point.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/Coordinate.h"
#include "geos/io/WKTReader.h"
#include "geos/io/WKTWriter.h"
#include "geos/io/ParseException.h"
#include "coordinate.h"
#include "location.h"

#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"

#include "geos/geom/CoordinateFilter.h"
#include "csytransform.h"

#include "geometryhelper.h"


using namespace Ilwis;

std::string GeometryHelper::toWKT(const geos::geom::Geometry* geom){
    geos::io::WKTWriter writer;
    return writer.write(geom);
}

geos::geom::Geometry* GeometryHelper::fromWKT(std::string wkt) throw(geos::io::ParseException){
    geos::io::WKTReader reader;
    return reader.read(wkt);
}

IlwisTypes GeometryHelper::geometryType(const geos::geom::Geometry *geom){
    switch ( geom->getGeometryTypeId()){
    case geos::geom::GEOS_POINT:
    case geos::geom::GEOS_MULTIPOINT:
        return itPOINT;
    case geos::geom::GEOS_LINESTRING:
    case geos::geom::GEOS_MULTILINESTRING:
        return itLINE;
    case geos::geom::GEOS_POLYGON:
    case geos::geom::GEOS_MULTIPOLYGON:
        return itPOLYGON;
    default:
        return  itUNKNOWN;
    }
}

void GeometryHelper::transform(geos::geom::Geometry *geom, const ICoordinateSystem& source, const ICoordinateSystem& target){
    CsyTransform trans(source, target);
    geom->apply_rw(&trans);
}

Ilwis::CoordinateSystem* GeometryHelper::getCoordinateSystem(geos::geom::Geometry *geom){
    if ( geom == nullptr)
        return 0;
    void *ptr = geom->getUserData();
    if (ptr == 0)
        return 0;
    return reinterpret_cast<Ilwis::CoordinateSystem *>(ptr);
}

void GeometryHelper::setCoordinateSystem(geos::geom::Geometry* geom, Ilwis::CoordinateSystem* csy){
    geom->setUserData(csy);
}

