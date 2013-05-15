#include <QString>

#include "kernel.h"
#include "ilwis.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "coordinatesystem.h"
#include "georeference.h"


using namespace Ilwis;

GeoReference::GeoReference() : _centerOfPixel(true)
{
}

GeoReference::GeoReference(const Resource& res) : IlwisObject(res), _centerOfPixel(true)
{
}

ICoordinateSystem GeoReference::coordinateSystem() const
{
    return _csy;
}

 void GeoReference::coordinateSystem(const ICoordinateSystem& csy)
{
     _csy = csy;
 }


Size GeoReference::size() const
{
    return _size;

}

void GeoReference::size(const Size &sz)
{
    // size must always be positive or undefined
    if (sz.xsize() > 0 && sz.ysize() > 0)
        _size = sz;


}

bool GeoReference::centerOfPixel() const
{
    return _centerOfPixel;
}

void GeoReference::centerOfPixel(bool yesno)
{
    _centerOfPixel = true;
}


