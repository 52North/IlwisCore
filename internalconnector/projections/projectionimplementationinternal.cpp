#include <QString>
#include <functional>
#include <QtMath>

#include "kernel.h"
#include "ilwis.h"
#include "geos/geom/Coordinate.h"
#ifdef Q_OS_WIN
#include "geos/geom/Coordinate.inl"
#endif
#include "coordinate.h"
#include "location.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "box.h"
#include "coordinatesystem.h"
#include "conventionalcoordinatesystem.h"
#include "projectionimplementation.h"
#include "projectionimplementationinternal.h"

using namespace Ilwis;
using namespace Internal;

ProjectionImplementationInternal::ProjectionImplementationInternal(const Resource& resource) :
    ProjectionImplementation(resource.code()),
    _easting(0),
    _northing(0),
    _maxis(6371007.180918499800),
    _centralMeridian(0)
{
}

Coordinate ProjectionImplementationInternal::latlon2coord(const LatLon &ll) const
{
    if (_coordinateSystem->projection().isValid() && ll.isValid()) {
        LatLon pl(ll);
        if (pl.lat() > M_PI_2)
            pl.lat(Angle(M_PI_2, true));
        else if (pl.lat() < -M_PI_2)
            pl.lat(Angle(-M_PI_2, true));
        pl.lon( pl.lon()-_centralMeridian);
        Coordinate xy = ll2crd(pl);
        if (xy != crdUNDEF)
            return crdUNDEF;
        Coordinate crd;
        crd.x = xy.x * _maxis  + _easting;
        crd.y = xy.y * _maxis  + _northing;
        return crd;
    }
    else
        return crdUNDEF;

}

LatLon ProjectionImplementationInternal::coord2latlon(const Ilwis::Coordinate &crdSource) const
{
    if (_coordinateSystem->projection().isValid() && crdSource != crdUNDEF) {
        Coordinate xy((crdSource.x - _easting) / _maxis, (crdSource.y - _northing) / _maxis);

        LatLon pl = crd2ll(xy);
        if (!pl.isValid())
            return llUNDEF;
        if (abs(pl.lat()) > 90)
            return llUNDEF;
        pl.lon( pl.lon() + _centralMeridian);
        return pl;
    }
    else
        return llUNDEF;

}

void ProjectionImplementationInternal::setCoordinateSystem(ConventionalCoordinateSystem *csy)
{
    ProjectionImplementation::setCoordinateSystem(csy);
    IProjection prj = _coordinateSystem->projection();
    if ( prj.isValid()) {
        _easting = prj->parameter(Projection::pvX0).toDouble();
        _northing = prj->parameter(Projection::pvY0).toDouble();
        _maxis = _coordinateSystem->ellipsoid()->majorAxis();
        _centralMeridian = prj->parameter(Projection::pvLON0).toDouble();
    }


}

QString ProjectionImplementationInternal::toProj4() const
{
    return sUNDEF; // TODO:

}

bool ProjectionImplementationInternal::canConvertToLatLon() const
{
    return true;
}

bool ProjectionImplementationInternal::canConvertToCoordinate() const
{
    return true;
}
