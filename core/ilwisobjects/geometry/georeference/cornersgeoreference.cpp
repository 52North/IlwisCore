#include "kernel.h"
#include "geometries.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"

using namespace Ilwis;

CornersGeoReference::CornersGeoReference() : SimpelGeoReference("corners")
{

}

GeoRefImplementation *CornersGeoReference::create()
{
    return new CornersGeoReference();
}

void CornersGeoReference::copyTo(GeoRefImplementation *impl)
{
    SimpelGeoReference::copyTo(impl);
    static_cast<CornersGeoReference *>(impl)->_envelope = _envelope;
}

void CornersGeoReference::envelope(const Envelope &env)
{
    _envelope = env;
}

bool CornersGeoReference::compute()
{
    bool a = size().isNull();
    bool b = _envelope.isValid();
    if (a || !b)
        return false;

    _a12 = _a21 = 0;
    std::vector<double> vec = _envelope.max_corner() - _envelope.min_corner();

    bool deltaxSmall = (std::abs(vec[0]) < 0.0000001);
    bool deltaySmall = (std::abs(vec[1]) < 0.0000001);
    if ( deltaxSmall || deltaySmall) {
        return false;
    }
    if (!_centerOfPixel) { // corners of corner pixels
        _a11  = size().xsize() / vec[0];
        double tempy = size().ysize();
        _a22 = - tempy / vec[1];
        _b1 = - _a11 * _envelope.min_corner().x;
        _b2 = - _a22 * _envelope.max_corner().y;
    }
    else { // center of corner pixels
        _a11 = (size().xsize() - 1) / vec[0];
        double v1 = size().ysize() - 1;
        double v2 = vec[1];
        double v3 = -v1/v2;
        _a22 = v3;
        _b1 = 0.5 - _a11 * _envelope.min_corner().x;
        _b2 = 0.5 - _a22 * _envelope.max_corner().y;
    }
    _det = _a11 * _a22;


    return true;
}

Envelope CornersGeoReference::envelope() const
{
    return _envelope;
}

GeoRefImplementation *CornersGeoReference::clone()
{
    GeoRefImplementation *impl = new CornersGeoReference();
    copyTo(impl);
    return impl;
}

QString CornersGeoReference::typeName()
{
    return "corners";
}



