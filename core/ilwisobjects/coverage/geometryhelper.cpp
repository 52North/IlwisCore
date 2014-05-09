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
#include "geos/io/WKTReader.inl"
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

QString GeometryHelper::toWKT(const geos::geom::Geometry* geom){
    geos::io::WKTWriter writer;
    return QString::fromStdString(writer.write(geom));
}

geos::geom::Geometry* GeometryHelper::fromWKT(const QString& wkt) {
    try{
        geos::io::WKTReader reader;
        geos::geom::Geometry* geom = reader.read(wkt.toStdString());
        if (geom)
            return geom;
        ERROR2(ERR_NOT_COMPATIBLE2, "WKT", TR("this wkt parser"));
        return 0;
    } catch (const geos::io::ParseException& ex){
        ERROR1(ERR_OPERATION_FAILID1, "Well Known Text");
        return 0;
    }
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
	case geos::geom::GEOS_LINEARRING:		    
        return itPOLYGON;
    case geos::geom::GEOS_GEOMETRYCOLLECTION:
        return itPOLYGON|itLINE|itPOINT;
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

