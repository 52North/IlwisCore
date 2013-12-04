#ifndef PROJECTION_H
#define PROJECTION_H

#include "Kernel_global.h"

namespace Ilwis {

class ProjectionImplementation;
class ConventionalCoordinateSystem;

class KERNELSHARED_EXPORT Projection : public IlwisObject
{
public:
     enum ProjectionParamValue { pvNONE, pvX0, pvY0, pvLON0,
     pvLATTS, pvLAT0, pvK0, pvNORTH,
     pvZONE, pvLAT1, pvLAT2, pvHEIGHT,
     pvTILTED, pvTILT, pvAZIMYAXIS, pvAZIMCLINE,
     pvPOLE, pvNORIENTED, pvELLCODE, pvLAST };

    enum FindMatch{fmNoItem, fmNoValue,fmMatch};

    Projection();
    virtual ~Projection();

    virtual Coordinate latlon2coord(const LatLon&) const;
    virtual LatLon coord2latlon(const Coordinate&) const;

    bool prepare(const QString& parms);
    bool prepare();
    void setImplementation(ProjectionImplementation *impl);

    QString authority() const;
    void setAuthority(const QString &auth);
    QString wkt() const;
    void setWkt(const QString &wkt);
    QString toProj4() const;

    IlwisTypes ilwisType() const;

    QVariant parameter(Projection::ProjectionParamValue type) const;
    void setParameter(Projection::ProjectionParamValue type, const QVariant& value);
    QString parameterName(Projection::ProjectionParamValue pv) const;

    bool isEqual(const IlwisObject* obj) const;
    bool isValid() const;

    void setCoordinateSystem(ConventionalCoordinateSystem *csy);
protected:
    static Projection::FindMatch find(const QString& v1, const QString& value) ;

private:
    QString _wkt;
    QString _authority;
    QScopedPointer<ProjectionImplementation> _implementation;
};
}

typedef Ilwis::IlwisData<Ilwis::Projection> IProjection;

#endif // PROJECTION_H
