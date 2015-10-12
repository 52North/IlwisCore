#include <QString>

#include "kernel.h"
#include "ilwis.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "georefimplementation.h"

using namespace Ilwis;

GeoRefImplementation::GeoRefImplementation(const QString &type) : Identity(type), _centerOfPixel(true)
{
}

GeoRefImplementation::~GeoRefImplementation()
{
    _csy.set(0);
}

ICoordinateSystem GeoRefImplementation::coordinateSystem() const
{
    return _csy;
}

 void GeoRefImplementation::coordinateSystem(const ICoordinateSystem& csy)
{
    _csy = csy;
}


Size<> GeoRefImplementation::size() const
{
    return _size;
}

void GeoRefImplementation::size(const Size<> &sz)
{
    // size must always be positive or undefined
    if (sz.xsize() > 0 && sz.ysize() > 0)
        _size = sz.toQSize();
}

Envelope GeoRefImplementation::envelope() const
{
    return Envelope();
}

void GeoRefImplementation::envelope(const Envelope &)
{

}

bool GeoRefImplementation::centerOfPixel() const
{
    return _centerOfPixel;
}

void GeoRefImplementation::centerOfPixel(bool yesno)
{
    _centerOfPixel = yesno;
}

bool GeoRefImplementation::compute()
{
    return true;
}


bool GeoRefImplementation::isCompatible(const IGeoReference &georefOther) const
{
    return false;
}

void GeoRefImplementation::copyTo(GeoRefImplementation *impl)
{
    impl->_csy = _csy;
    impl->_centerOfPixel = _centerOfPixel;
    impl->_size = _size;
    impl->name( name());
    impl->setDescription(description());
    impl->code(code());
}
