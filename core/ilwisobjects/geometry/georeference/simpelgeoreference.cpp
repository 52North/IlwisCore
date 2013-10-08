#include <QString>

#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"


using namespace Ilwis;

SimpelGeoReference::SimpelGeoReference(const QString& type) : GeoRefImplementation(type)
{
    clear();
}

SimpelGeoReference::SimpelGeoReference() : GeoRefImplementation("simpel")
{
    clear();
}

GeoRefImplementation *SimpelGeoReference::create()
{
    return new SimpelGeoReference();
}

void SimpelGeoReference::clear() {
    _a11 = _a12 = _a22 = _a21 = rUNDEF;
    _b1 = _b2 = rUNDEF;
    _det = 0;
}

Coordinate SimpelGeoReference::pixel2Coord(const Pixel_d &pix) const
{
    double x = centerOfPixel() ? pix.x() + 0.5 : pix.x();
    double y = centerOfPixel()? pix.y() + 0.5 : pix.y();
    Coordinate c((_a22 * (x - _b1) - _a12 * (y - _b2)) / _det,
                 (-_a21 * (x - _b1) + _a11 * (y - _b2)) / _det );
    return c;
}

Pixel_d SimpelGeoReference::coord2Pixel(const Coordinate &crd) const
{
    double rCol = _a11 * crd.x() + _a12 * crd.y() + _b1; // - 1;
    double rRow = _a21 * crd.x() + _a22 * crd.y() + _b2; // - 1;
    Pixel_d pix(rCol, rRow);

    return pix;
}

double SimpelGeoReference::pixelSize() const
{
    if ( _det == 0)
        return rUNDEF;

    return sqrt( 1/ _det);
}

std::vector<double> SimpelGeoReference::matrix() const {
    return {_a11,_a21,_a12,_a22};
}

std::vector<double> SimpelGeoReference::support() const {
    return {_b1, _b2};
}

QString SimpelGeoReference::typeName()
{
    return "simple";
}

bool SimpelGeoReference::isCompatible(const IGeoReference &georefOther) const
{

    if ( !georefOther->grfType<SimpelGeoReference>())
        return false;
    QSharedPointer<SimpelGeoReference> grfsmpl = georefOther->impl<SimpelGeoReference>();
    double delta = EPS6;
    const std::vector<double>& mat = grfsmpl->matrix();
    bool ok = std::abs(mat[0] - _a11) < delta && std::abs(mat[1] - _a21) < delta &&
            std::abs(mat[2] - _a12) < delta && delta && std::abs(mat[3] - _a22) < delta;
    if (!ok)
        return false;
    const std::vector<double>& sup = grfsmpl->support();
    ok = std::abs(sup[0] - _b1) < delta && std::abs(sup[1] - _b2) < delta;

    return ok;

}









