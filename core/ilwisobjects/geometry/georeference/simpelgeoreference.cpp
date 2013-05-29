#include <QString>

#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "simpelgeoreference.h"


using namespace Ilwis;

SimpelGeoReference::SimpelGeoReference()
{
    clear();
}

SimpelGeoReference::SimpelGeoReference(const Resource &res) :
    GeoReference(res),
    _det(0)
{
    clear();
}

void SimpelGeoReference::clear() {
    _a11 = _a12 = _a22 = _a21 = rUNDEF;
    _b1 = _b2 = rUNDEF;
}

Coordinate SimpelGeoReference::pixel2Coord(const Pixel &rc) const
{
    double x = centerOfPixel() ? rc.x() + 0.5 : rc.x();
    double y = centerOfPixel()? rc.y() + 0.5 : rc.y();
    Coordinate c((_a22 * (x - _b1) - _a12 * (y - _b2)) / _det,
                 (-_a21 * (x - _b1) + _a11 * (y - _b2)) / _det );
    return c;
}

Point2D<double> SimpelGeoReference::coord2Pixel(const Coordinate &crd) const
{
    double rCol = _a11 * crd.x() + _a12 * crd.y() + _b1; // - 1;
    double rRow = _a21 * crd.x() + _a22 * crd.y() + _b2; // - 1;
    return Point2D<double>(rCol, rRow);
}

Box2D<double> SimpelGeoReference::pixel2Coord(const Box2D<qint32> &box) const
{
    if ( !box.isValid()) {
        ERROR2(ERR_INVALID_PROPERTY_FOR_2,"size", "box");
        return Box2D<double>();
    }
    Coordinate c1 = pixel2Coord(box.min_corner());
    Coordinate c2 = pixel2Coord(box.max_corner());
    return Box2D<double>(c1,c2);
}

Box2D<qint32> SimpelGeoReference::coord2Pixel(const Box2D<double> &box) const
{
    if ( !box.isValid()) {
        ERROR2(ERR_INVALID_PROPERTY_FOR_2,"size", "box");
        return  Box2D<qint32>();
    }
    Coordinate xxx(box.min_corner());
    Pixel p1 = coord2Pixel(box.min_corner());
    Pixel p2 = coord2Pixel(box.max_corner());
    return Box2D<qint32>(p1,p2);
}

double SimpelGeoReference::pixelSize() const
{
    double d = _det;
    return sqrt( 1/ d);
}

std::vector<double> SimpelGeoReference::matrix() const {
    return {_a11,_a21,_a21,_a22};
}

std::vector<double> SimpelGeoReference::support() const {
    return {_b1, _b2};
}









