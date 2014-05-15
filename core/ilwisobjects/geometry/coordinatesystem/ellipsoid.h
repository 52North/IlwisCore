#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include "kernel_global.h"

namespace Ilwis {
class Ellipsoid;

typedef IlwisData<Ilwis::Ellipsoid> IEllipsoid;
#define WGS84RESOURCE QString("ilwis://tables/ellipsoid?code=WE")

class KERNELSHARED_EXPORT Ellipsoid : public IlwisObject
{
public:
    Ellipsoid();
    Ellipsoid(const Resource &resource);
    virtual ~Ellipsoid();

    bool isSpherical() const;
    bool isEqual(const IEllipsoid &ell) const;
    bool isValid() const;
    double distance(const LatLon& begin, const LatLon &end) const;
    double azimuth(const LatLon &begin, const LatLon &end) const;
    double majorAxis() const;
    double minorAxis() const;
    double flattening() const;
    double excentricity() const;
    double excentricity2() const;
    void setEllipsoid(double a, double f, bool setCodeToo=true);
    QString authority() const;
    void setAuthority(const QString &auth);
    QString toProj4() const;
    IlwisTypes ilwisType() const;

    LatLon latlon2Coord(const IEllipsoid &sourceEllipsoide, const LatLon &sourceLatLon) const;
    Coordinate latlon2Coord(const LatLon &sourceLatLon) const;
    LatLon coord2latlon(const Coordinate &crd) const;
    Coordinate coord2coord(const Coordinate& ctsIn, const Coordinate& ctsPivot,
                                                            double tx, double ty, double tz,
                                                            double Rx, double Ry, double Rz,
                                                            double s) const;

    static const IEllipsoid wgs84ellipsoid;

private:
    Ellipsoid(double a, double f);

    double _flattening;
    double _majorAxis;
    double _minoraxis;
    double _excentricity;
    QString _authority;
};

}

#endif // ELLIPSOID_H
