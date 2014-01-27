#include <functional>

#include "kernel.h"
#include "ilwis.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "coordinatesystem.h"
#include "conventionalcoordinatesystem.h"
#include "ProjectionImplementation.h"
#include "proj4parameters.h"

using namespace Ilwis;

ProjectionImplementation::ProjectionImplementation(const QString &type) :
    _coordinateSystem(0),
    _projtype(type)
{
    _parameters[Projection::Projection::pvX0] = 0;
    _parameters[Projection::pvY0] = 0;
    _parameters[Projection::pvK0] = 0;
    _parameters[Projection::pvLAT0] = 0;
    _parameters[Projection::pvLAT1] = 0;
    _parameters[Projection::pvLAT2] = 0;
    _parameters[Projection::pvZONE] = 1;
    _parameters[Projection::pvNORIENTED] =  true;
    _parameters[Projection::pvAZIMYAXIS] = 0;
    _parameters[Projection::pvTILTED] =  false;
    _parameters[Projection::pvAZIMCLINE] = 0;
    _parameters[Projection::pvHEIGHT] = 0;
    _parameters[Projection::pvLON0] =  0;
}

QString ProjectionImplementation::type() const
{
    return _projtype;
}

void ProjectionImplementation::setCoordinateSystem(ConventionalCoordinateSystem *csy)
{
    _coordinateSystem = csy;
}

QVariant ProjectionImplementation::parameter(Projection::ProjectionParamValue type) const
{
    if ( _parameters.contains(type)) {
        return _parameters.value(type);
    }
    return QVariant();
}

void ProjectionImplementation::setParameter(Projection::ProjectionParamValue type, const QVariant &value)
{
    _parameters[type] = value;
}

bool ProjectionImplementation::isEqual(const QScopedPointer<ProjectionImplementation>& projimpl) {
    QString proj4a = toProj4();
    QString proj4b = projimpl->toProj4();

    return proj4a == proj4b;
}


