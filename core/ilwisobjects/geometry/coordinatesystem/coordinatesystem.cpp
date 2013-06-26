#include <QString>

#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
//#include "domain.h"
#include "coordinatesystem.h"

using namespace Ilwis;

CoordinateSystem::CoordinateSystem()
{
}

CoordinateSystem::CoordinateSystem(const Resource& res) : IlwisObject(res)
{
}

Box2D<double> CoordinateSystem::convertEnvelope(const ICoordinateSystem &sourceCs, const Box2D<double> &envelope) const
{
    double rDX = envelope.width()/10.0;
    double rDY = envelope.height()/10.0;
    int iX, iY;
    Coordinate cXY;
    Box2D<double> env;
    for ( cXY.x(envelope.min_corner().x()),iX = 0; iX <= 10; cXY += {rDX, 0}, ++iX) {
        for (cXY.y(envelope.min_corner().y()),iY = 0; iY <= 10; cXY += {0,rDY} , ++iY ) {
            Coordinate crdNew = coord2coord(sourceCs, cXY);
            env += crdNew;
        }
    }
    return env;
}

bool CoordinateSystem::canConvertToLatLon() const
{
    return false;
}

bool CoordinateSystem::canConvertToCoordinate() const
{
    return false;
}

Coordinate CoordinateSystem::inverseCoordinateConversion(const CoordinateSystem& cs, const Coordinate &crd) const
{
    if (cs.isEqual(cs))
      return crd;
    return Coordinate();
}



