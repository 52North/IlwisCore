#include "kernel.h"
#include "ilwisangle.h"
#include "geometries.h"
#include "georefadapter.h"

using namespace Ilwis;

GeoRefAdapter::GeoRefAdapter()
{
}

//-------------------------------------------------
FactorAdapter::FactorAdapter(std::vector<double>& offsets, double fac) :
    _offsets(offsets),_factor(fac)
{
}

Pixeld FactorAdapter::adaptBefore(const Pixeld &inPix) const
{
    Pixeld outPix = inPix / _factor;
    return outPix + _offsets;
}

Pixeld FactorAdapter::adaptAfter(const Pixeld& inPix) const
{
    Pixeld outPix = inPix - _offsets;
    return outPix * _factor;
}

//--------------------------------------------------


ScaleRotateAdapter::ScaleRotateAdapter(const std::vector<double> &pivot, const std::vector<double> &targetOffet, double angle, double scale) :
    _pivot(pivot), _offset(targetOffet), _angle(angle), _scale(scale)
{
}

Pixeld ScaleRotateAdapter::adaptBefore(const Pixeld &inPix) const
{
    Pixeld pix = inPix - _offset;
    double y = cos(_angle) / _scale * pix.y - sin(_angle) / _scale * pix.x;
    double x = sin(_angle) / _scale * pix.y - cos(_angle) / _scale * pix.x;

    return Pixeld(x,y)  + _offset;

}


Pixeld ScaleRotateAdapter::adaptAfter(const Pixeld &inPix) const
{
    Pixeld pixTemp = inPix - _pivot;
    double x = -sin(_angle) * _scale * pixTemp.y + cos(_angle) * _scale * pixTemp.x;
    double y = cos(_angle) * _scale * pixTemp.y + sin(_angle) * _scale * pixTemp.x;

    return Pixeld(x,y) + _offset;

}

//----------------------------------------------------

