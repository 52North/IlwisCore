#ifndef COORDINATESYSTEM_H
#define COORDINATESYSTEM_H

#include "Kernel_global.h"

namespace Ilwis {

class CoordinateSystem;
template<class T> class Box2D;

typedef IlwisData<CoordinateSystem> ICoordinateSystem;

class KERNELSHARED_EXPORT CoordinateSystem : public IlwisObject
{
public:
    CoordinateSystem();
    CoordinateSystem(const Ilwis::Resource &resource);

    virtual Coordinate coord2coord(const ICoordinateSystem& sourceCs, const Coordinate& crdSource) const =0;
    virtual LatLon coord2latlon(const Coordinate &crdSource) const =0;
    virtual Coordinate latlon2coord(const LatLon& ll) const = 0;
    virtual Ilwis::Box2D<double> convertEnvelope(const ICoordinateSystem& sourceCs, const Ilwis::Box2D<double>& envelope) const;
    virtual bool canConvertToLatLon() const;
    virtual bool canConvertToCoordinate() const;
    virtual Coordinate inverseCoordinateConversion(const CoordinateSystem& cs, const Coordinate& crd) const;
    Box3D<double> envelope() const;
    void envelope(const Box3D<double> &env);
    virtual bool isLatLon() const = 0;

private:
    Box3D<double> _envelope;

};

}

Q_DECLARE_METATYPE(Ilwis::ICoordinateSystem)


#endif // COORDINATESYSTEM_H
