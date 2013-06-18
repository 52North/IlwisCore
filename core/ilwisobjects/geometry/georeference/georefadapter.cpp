#include "kernel.h"
#include "angle.h"
#include "point.h"
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

Pixel_d FactorAdapter::adaptBefore(const Pixel_d &inPix) const
{
    Pixel_d outPix = inPix / _factor;
    return outPix + _offsets;
}

Pixel_d FactorAdapter::adaptAfter(const Pixel_d& inPix) const
{
    Pixel_d outPix = inPix - _offsets;
    return outPix * _factor;
}

//--------------------------------------------------


ScaleRotateAdapter::ScaleRotateAdapter(const std::vector<double> &pivot, const std::vector<double> &targetOffet, double angle, double scale) :
    _pivot(pivot), _offset(targetOffet), _angle(angle), _scale(scale)
{
}

Pixel_d ScaleRotateAdapter::adaptBefore(const Pixel_d &inPix) const
{
    Pixel_d pix = inPix - _offset;
    double y = cos(_angle) / _scale * pix.y() - sin(_angle) / _scale * pix.x();
    double x = sin(_angle) / _scale * pix.y() - cos(_angle) / _scale * pix.x();

    return Pixel_d(x,y)  + _offset;

}


Pixel_d ScaleRotateAdapter::adaptAfter(const Pixel_d &inPix) const
{
    Pixel_d pixTemp = inPix - _pivot;
    double x = -sin(_angle) * _scale * pixTemp.y() + cos(_angle) * _scale * pixTemp.x();
    double y = cos(_angle) * _scale * pixTemp.y() + sin(_angle) * _scale * pixTemp.x();

    return Pixel_d(x,y) + _offset;

}

//----------------------------------------------------

