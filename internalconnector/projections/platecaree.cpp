#include <QString>
#include <functional>

#include "kernel.h"
#include "ilwis.h"
 //#include "angle.h"
//#include "point.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "geos/geom/Coordinate.h"
#include "ilwiscoordinate.h"
#include "location.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "projectionimplementation.h"
#include "box.h"
#include "coordinatesystem.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "projectionimplementationinternal.h"
#include "platecaree.h"

using namespace Ilwis;
using namespace Internal;

PlateCaree::PlateCaree(const Ilwis::Resource &resource) : ProjectionImplementationInternal(resource)
{

}

PlateCaree::~PlateCaree()
{
}

Coordinate PlateCaree::ll2crd(const LatLon &ll) const
{
    return Coordinate(ll.lon(), ll.lat());
}

LatLon PlateCaree::crd2ll(const Coordinate &crd) const
{
    LatLon ll(crd.y * 180.0 / M_PI, crd.x * 180.0 / M_PI);
    return ll;
}

bool PlateCaree::canUse(const Ilwis::Resource &resource)
{
    QString prj = resource.code();
    if ( prj == sUNDEF)
        return false;
    if ( prj == "PRJPC")
        return true;

    return false;
}

bool PlateCaree::prepare(const QString& parms){
    return true;
}

ProjectionImplementation *PlateCaree::clone()
{
    PlateCaree *csy = new PlateCaree();
    copyTo(csy);
    return csy;
}

PlateCaree::PlateCaree()
{

}



