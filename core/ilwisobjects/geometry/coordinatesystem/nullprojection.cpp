#include <QString>

#include "kernel.h"
#include "angle.h"
#include "point.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "projectionimplementation.h"
#include "nullprojection.h"

using namespace Ilwis;

NullProjection::NullProjection(const Resource& resource)
{
    setName(resource.name());
    setCode(resource.code());
    setDescription("Null Projection");
}

Coordinate NullProjection::latlon2coord(const LatLon &ll) const
{
    return Coordinate(ll.y(), ll.x(), ll.z());
}

LatLon NullProjection::coord2latlon(const Coordinate &crd) const
{
    return LatLon(crd.y(), crd.x(), crd.z());
}
