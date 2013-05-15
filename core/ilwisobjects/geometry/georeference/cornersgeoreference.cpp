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
#include "boost/numeric/ublas/matrix.hpp"
#include "boostext.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"

using namespace Ilwis;

CornersGeoReference::CornersGeoReference() : _isCornersOfCorners(false)
{

}

CornersGeoReference::CornersGeoReference(const Resource &res) :
    SimpelGeoReference(res),
    _isCornersOfCorners(false)
{
}

void CornersGeoReference::setEnvelope(const Box2D<double> &env)
{
    _envelope = env;
}

bool CornersGeoReference::compute()
{
    bool a = size().isNull();
    bool b = _envelope.isValid();
    if (a || !b)
        return setValid(false);

    _a12 = _a21 = 0;
    std::vector<double> vec = _envelope.max_corner() - _envelope.min_corner();

    if (abs(vec[0]) < 1e-6 || abs(vec[1]) < 1e-6) {
        return setValid(false);
    }
    if (_isCornersOfCorners) {
        _a11  = size().xsize() / vec[0];
        _a22 = - size().ysize() / vec[1];
        _b1 = - _a11 * _envelope.min_corner().x();
        _b2 = - _a22 * _envelope.max_corner().y();
    }
    else {
        _a11 = (size().xsize() - 1) / vec[0];
        _a22 = - (size().ysize() - 1) / vec[1];
        _b1 = 0.5 - _a11 * _envelope.min_corner().x();
        _b2 = 0.5 - _a22 * _envelope.max_corner().y();
    }
    _det = _a11 * _a22;
    setValid(true);

    return true;
}


IlwisTypes CornersGeoReference::ilwisType() const
{
    return itCORNERSGEOREF;
}

bool CornersGeoReference::isCornersOfCorners() const
{
    return _isCornersOfCorners;
}

Box2D<double> CornersGeoReference::envelope() const
{
    return _envelope;
}



