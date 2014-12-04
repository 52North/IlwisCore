#include <QString>

#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "boundsonlycoordinatesystem.h"

using namespace Ilwis;
BoundsOnlyCoordinateSystem::BoundsOnlyCoordinateSystem()
{
}

BoundsOnlyCoordinateSystem::BoundsOnlyCoordinateSystem(const Ilwis::Resource &resource) :  CoordinateSystem(resource)
{

}

Coordinate BoundsOnlyCoordinateSystem::coord2coord(const ICoordinateSystem &sourceCs, const Coordinate &crdSource) const
{
    if ( sourceCs->id() == id())
        return crdSource;
    return Coordinate();
}

LatLon BoundsOnlyCoordinateSystem::coord2latlon(const Coordinate &crdSource) const
{
    return LatLon();
}

Coordinate BoundsOnlyCoordinateSystem::latlon2coord(const LatLon &ll) const
{
    return Coordinate();
}

bool BoundsOnlyCoordinateSystem::isLatLon() const
{
    return false;
}

bool BoundsOnlyCoordinateSystem::isUnknown() const
{
    return  ( code() == "csy::unknown" || code() == "unknown");
}

IlwisTypes BoundsOnlyCoordinateSystem::ilwisType() const
{
    return itBOUNDSONLYCSY;
}

QString BoundsOnlyCoordinateSystem::toWKT(quint32 spaces) const
{
    return sUNDEF;
}
