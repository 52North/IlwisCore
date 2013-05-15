#ifndef COORDINATESYSTEMVIALATLON_H
#define COORDINATESYSTEMVIALATLON_H

#include "Kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT ConventionalCoordinateSystem : public CoordinateSystem
{
public:
    ConventionalCoordinateSystem();
    ConventionalCoordinateSystem(const Resource& res);

    Coordinate coord2coord(const ICoordinateSystem &sourceCs, const Coordinate& crdSource) const;
    LatLon coord2latlon(const Coordinate &crdSource) const;
    Coordinate latlon2coord(const LatLon& ll) const;
    const std::unique_ptr<Ilwis::GeodeticDatum> &datum() const;
    void setDatum(Ilwis::GeodeticDatum *datum);
    IEllipsoid ellipsoid() const;
    void setEllipsoid(const IEllipsoid& ell);
    void setProjection(const IProjection& proj);
    IProjection projection() const;
    virtual bool isLatLon() const;
    bool prepare(const QString& parms);
    bool prepare();
    IlwisTypes ilwisType() const;

private:
    IEllipsoid _ellipsoid;
    IProjection _projection;
    std::unique_ptr<GeodeticDatum> _datum;
    QString _unit;
};

typedef IlwisData<ConventionalCoordinateSystem> IConventionalCoordinateSystem;

}

#endif // COORDINATESYSTEMVIALATLON_H
