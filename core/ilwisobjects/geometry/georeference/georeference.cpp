#include <QString>

#include "kernel.h"
#include "ilwis.h"
#include "geometries.h"
#include "ilwisdata.h"
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

void GeoReference::adapter(GeoRefAdapter *adapt)
{
    _adapter.reset(adapt);
}

const QScopedPointer<Ilwis::GeoRefAdapter> &GeoReference::adapter() const
{
    return _adapter;
}

Box2D<double> GeoReference::pixel2Coord(const Box2D<qint32> &box) const
{
    if ( !box.isValid()) {
        ERROR2(ERR_INVALID_PROPERTY_FOR_2,"size", "box");
        return Box2D<double>();
    }
    Coordinate c1 = pixel2Coord(box.min_corner());
    Coordinate c2 = pixel2Coord(box.max_corner());
    return Box2D<double>(c1,c2);
}

Box2D<qint32> GeoReference::coord2Pixel(const Box2D<double> &box) const
{
    if ( !box.isValid()) {
        ERROR2(ERR_INVALID_PROPERTY_FOR_2,"size", "box");
        return  Box2D<qint32>();
    }
    Pixel p1 = coord2Pixel(box.min_corner());
    Pixel p2 = coord2Pixel(box.max_corner());
    return Box2D<qint32>(p1,p2);
}


