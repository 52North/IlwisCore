#ifndef COORDINATESYSTEM_H
#define COORDINATESYSTEM_H

#include "kernel_global.h"

namespace Ilwis {

class CoordinateSystem;
template<class T> class Box;

typedef IlwisData<CoordinateSystem> ICoordinateSystem;

class KERNELSHARED_EXPORT CoordinateSystem : public IlwisObject
{
public:
    CoordinateSystem();
    CoordinateSystem(const Ilwis::Resource &resource);

    virtual Coordinate coord2coord(const ICoordinateSystem& sourceCs, const Coordinate& crdSource) const =0;
    virtual LatLon coord2latlon(const Coordinate &crdSource) const =0;
    virtual Coordinate latlon2coord(const LatLon& ll) const = 0;
    virtual Ilwis::Envelope convertEnvelope(const ICoordinateSystem& sourceCs, const Envelope& envelope) const;
    virtual bool canConvertToLatLon() const;
    virtual bool canConvertToCoordinate() const;
    virtual Coordinate inverseCoordinateConversion(const CoordinateSystem& cs, const Coordinate& crd) const;
    Ilwis::Envelope envelope(bool tolatlon=false) const;
    void envelope(const Envelope &env);
    virtual bool isLatLon() const = 0;
    virtual bool isUnknown() const = 0;
    virtual QString toWKT(quint32 spaces=0) const=0;

protected:
    void copyTo(IlwisObject *obj);
private:
    Ilwis::Envelope _envelope;

};

}

Q_DECLARE_METATYPE(Ilwis::ICoordinateSystem);


#endif // COORDINATESYSTEM_H
