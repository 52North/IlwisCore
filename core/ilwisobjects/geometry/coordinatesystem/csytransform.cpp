#include "geos/geom/Coordinate.h"
#include "geos/geom/CoordinateFilter.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "coordinatesystem.h"
#include "csytransform.h"

using namespace Ilwis;

CsyTransform::CsyTransform(const ICoordinateSystem& source, const ICoordinateSystem &target ) : _source(source), _target(target)
{
}

CsyTransform::CsyTransform(CoordinateSystem *source, const ICoordinateSystem &target) :  _target(target)
{
    _source.set(source);
}

void CsyTransform::filter_rw(geos::geom::Coordinate *crd) const {
    *crd = _target->coord2coord(_source, *crd);
}
