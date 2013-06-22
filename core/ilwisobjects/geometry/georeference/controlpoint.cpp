#include "kernel.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "controlpoint.h"

using namespace Ilwis;

ControlPoint::ControlPoint(bool invalid) : _neverValid(invalid)
{
}

ControlPoint::ControlPoint(const Coordinate &crd, const Pixel &pix) : Coordinate(crd),_gridLocation(pix), _active(false)
{
}

ControlPoint::ControlPoint(const Coordinate &crd, const Pixel_d &pix) : Coordinate(crd), _gridLocation(pix), _active(false)
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

Pixel_d ControlPoint::gridLocation() const
{
    return _gridLocation;
}

void ControlPoint::gridLocation(const Pixel_d &pix)
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
