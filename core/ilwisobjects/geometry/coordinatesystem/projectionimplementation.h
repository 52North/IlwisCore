#ifndef ProjectionImplementationIMPLEMENTATION_H
#define ProjectionImplementationIMPLEMENTATION_H

#include "kernel_global.h"

namespace Ilwis {

class ConventionalCoordinateSystem;

struct ProjectionParameter {
    ProjectionParameter(){}
    ProjectionParameter(const QVariant& var, IlwisTypes datatype=itDOUBLE, bool isSet=false) : _value(var),_isSet(isSet), _valueType(datatype){}
    QVariant _value;
    bool _isSet=false;
    bool _isUsed=false;
    IlwisTypes _valueType;
};

class KERNELSHARED_EXPORT ProjectionImplementation
{
public:

    ProjectionImplementation(const QString& type=sUNDEF);

    virtual ~ProjectionImplementation() {}

    virtual Coordinate latlon2coord(const LatLon&) const = 0;
    virtual LatLon coord2latlon(const Coordinate&) const = 0;
    virtual bool prepare(const QString& parms="")=0;
    virtual QString type() const;
    virtual void setCoordinateSystem(ConventionalCoordinateSystem *csy);

    QVariant parameter(Projection::ProjectionParamValue type) const;
    virtual void setParameter(Projection::ProjectionParamValue type, const QVariant& value);
    QString parameterName(Projection::ProjectionParamValue pv) const;
    virtual QString toProj4() const = 0;
    virtual bool canConvertToLatLon() const = 0;
    virtual bool canConvertToCoordinate() const = 0;


    bool isEqual(const QScopedPointer<ProjectionImplementation> &projimpl);
    QString toWKT(quint32 spaces=0);
    bool isSet(Projection::ProjectionParamValue type) const;
    bool isUsed(Projection::ProjectionParamValue type) const;
    IlwisTypes valueType(Projection::ProjectionParamValue type) const;
    virtual ProjectionImplementation* clone() = 0;
protected:
    virtual void copyTo(ProjectionImplementation *prj);
    void initParameterList(const QString& projtype);
    ConventionalCoordinateSystem *_coordinateSystem;

    std::map<Projection::ProjectionParamValue, ProjectionParameter> _parameters;
    QString _projtype;
};
}


#endif // ProjectionImplementationIMPLEMENTATION_H
