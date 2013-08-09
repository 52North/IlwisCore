#include <QUrl>
#include "module.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "domain.h"
#include "coordinatesystem.h"
#include "coverage.h"
#include "coordinatedomain.h"

using namespace Ilwis;

CoordinateDomain::CoordinateDomain()
{
}

CoordinateDomain::CoordinateDomain(const Resource &res) : Domain(res)
{
    if ( hasType(res.ilwisType(),itCOVERAGE)) {
        ICoverage cov;
        if( cov.prepare(res)) {
            _csy = cov->coordinateSystem();
            _envelope.reset(cov->envelope().clone());
        }
    } else if ( hasType(res.ilwisType(),itCOORDSYSTEM)) {
        _csy.prepare(res);
    }
}

void CoordinateDomain::setRange(Range *vr)
{
    _envelope = QSharedPointer<Range>(vr);
}

Domain::Containement CoordinateDomain::contains(const QVariant &value) const
{
    Coordinate crd = value.value<Coordinate>();
    if ( _envelope->valueType() == itCOORD2D) {
        QSharedPointer<Box2D<double>> box = _envelope.dynamicCast<Box2D<double>>();
        if ( box)
            return box->contains(crd) ? Domain::cSELF : Domain::cNONE;
    }
    if ( _envelope->valueType() == itCOORD3D) {
        QSharedPointer<Box3D<double>> box = _envelope.dynamicCast<Box3D<double>>();
        if ( box)
            return box->contains(crd) ? Domain::cSELF : Domain::cNONE;
    }

    return Domain::cNONE;
}


IlwisTypes CoordinateDomain::ilwisType() const
{
    return itCOORDDOMAIN;
}

ICoordinateSystem CoordinateDomain::coordinateSystem() const
{
    return _csy;
}

SPRange CoordinateDomain::getRange() const
{
    return _envelope;
}


IlwisTypes CoordinateDomain::valueType() const
{
    return itCOORD3D;
}


QString CoordinateDomain::value(const QVariant &v) const
{
    Coordinate crd = v.value<Coordinate>();
    QString c = QString("%1 %2 %3").arg(crd.x(), 0, 'g').arg(crd.y(), 0, 'g').arg(crd.z(), 0, 'g');
    return c;
}
