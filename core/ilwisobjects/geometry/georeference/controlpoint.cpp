#include "kernel.h"
#include "geometries.h"
#include "controlpoint.h"

using namespace Ilwis;

ControlPoint::ControlPoint(bool invalid) : _neverValid(invalid)
{
}

ControlPoint::ControlPoint(const Coordinate &crd, const Pixel &pix) : Coordinate(crd),_gridLocation(pix), _active(false)
{
}

ControlPoint::ControlPoint(const Coordinate &crd, const Pixeld &pix) : Coordinate(crd), _gridLocation(pix), _active(false)
{
}

ControlPoint::ControlPoint(const Coordinate &crd, const LatLon &ll) : Coordinate(crd), _llLocation(ll), _active(false)
{
}

LatLon ControlPoint::llLocation() const
{
    return _llLocation;
}

void ControlPoint::llLocation(const LatLon &ll)
{
    _llLocation = ll;
}

Pixeld ControlPoint::gridLocation() const
{
    return _gridLocation;
}

void ControlPoint::gridLocation(const Pixeld &pix)
{
    _gridLocation = pix;
}

bool ControlPoint::isActive() const
{
    return _active;
}

void ControlPoint::active(bool yesno)
{
    _active = yesno;
}

bool ControlPoint::isValid() const
{
    return !_neverValid && Coordinate::isValid() && _gridLocation.isValid();
}
