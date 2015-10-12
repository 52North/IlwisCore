#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "undeterminedgeoreference.h"

using namespace Ilwis;

GeoRefImplementation *UndeterminedGeoReference::create()
{
    return new UndeterminedGeoReference();
}

QString UndeterminedGeoReference::typeName()
{
    return "UndeterminedGeoReference";
}

UndeterminedGeoReference::UndeterminedGeoReference() : GeoRefImplementation("undetermined")
{
}

Coordinate UndeterminedGeoReference::pixel2Coord(const Pixeld &pix) const
{
    if ( !pix.isValid())
        return Coordinate();
    if (pix.is3D())
        return Coordinate(pix.x, pix.y, pix.z);
    return Coordinate(pix.x, pix.y);
}

Pixeld UndeterminedGeoReference::coord2Pixel(const Coordinate &crd) const
{
    if ( !crd.isValid())
        return Pixeld();
    if ( crd.is3D())
        return Pixeld(crd.x, crd.y, crd.z);
    return Pixeld(crd.x, crd.y);
}

double UndeterminedGeoReference::pixelSize() const
{
    return rUNDEF;
}

GeoRefImplementation *UndeterminedGeoReference::clone()
{
    return new UndeterminedGeoReference();
}


