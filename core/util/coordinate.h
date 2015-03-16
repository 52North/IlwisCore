#ifndef COORDINATE_H
#define COORDINATE_H

#include "angle.h"

namespace geos{
    namespace geom{
    class CoordinateSequence;
    class Coordinate;
    }
}

namespace Ilwis {
class KERNELSHARED_EXPORT Coordinate : public geos::geom::Coordinate{
public:
    Coordinate();
    Coordinate(double px, double py, double pz=rUNDEF);
    Coordinate(const geos::geom::Coordinate& crd);
    Coordinate(const Ilwis::Coordinate& crd);
    Coordinate(const std::vector<double> &v);
    Coordinate(const QVector3D& crd);
    virtual ~Coordinate();

    double undefined() const { return  rUNDEF; }
    quint64 valuetype() const { return itDOUBLE;}
    Coordinate& operator=(const geos::geom::Coordinate&);
    bool isValid() const;
    bool is0() const;
    bool is3D() const;
    operator std::vector<double>();
    Ilwis::Coordinate &operator =(const Ilwis::Coordinate &p2);
    Ilwis::Coordinate &operator =(const Ilwis::Coordinate&& p2);
    Ilwis::Coordinate &operator =(const QVector3D& p2);
    Coordinate &operator +=(const std::vector<double> &vec);
    Coordinate &operator -=(const std::vector<double> &vec);
    double distance(const Coordinate &crd);
    Ilwis::Coordinate &operator *=(const std::vector<double> &vec);
    Ilwis::Coordinate &operator *=(double v);
    Ilwis::Coordinate &operator /=(double v);
    bool operator ==(const Ilwis::Coordinate &pnt) const;
    bool operator ==(const geos::geom::Coordinate &pnt) const;
    bool operator !=(const Ilwis::Coordinate &pnt);
};

class KERNELSHARED_EXPORT LatLon : public Ilwis::Coordinate {
public:
    LatLon();
    LatLon(const Angle& latd, const Angle& lond, double h=0);
    LatLon(const QString& lat, const QString& lon);
    Angle lat() const;
    Angle lon() const;

    void lat(const Angle& lat);
    void lon(const Angle& lon);
};

#define llUNDEF  Ilwis::LatLon(rUNDEF, rUNDEF)

KERNELSHARED_EXPORT std::vector<double> operator-(const Ilwis::Coordinate& p1, const Ilwis::Coordinate& p2);
KERNELSHARED_EXPORT Ilwis::Coordinate operator+(const Ilwis::Coordinate& p1, const std::vector<double>& vec);
KERNELSHARED_EXPORT Ilwis::Coordinate operator-(const Ilwis::Coordinate& p1, const std::vector<double>& vec);
KERNELSHARED_EXPORT Ilwis::Coordinate operator*(const Ilwis::Coordinate& p1, double v);
KERNELSHARED_EXPORT Ilwis::Coordinate operator/(const Ilwis::Coordinate& p1, double v);
}

#define crdUNDEF Ilwis::Coordinate(rUNDEF, rUNDEF, rUNDEF)
typedef std::unique_ptr<geos::geom::CoordinateSequence *> UPCoordinates;



Q_DECLARE_METATYPE(Ilwis::Coordinate)

#endif // COORDINATE_H

