#ifndef BOUNDSONLYCOORDINATESYSTEM_H
#define BOUNDSONLYCOORDINATESYSTEM_H

namespace Ilwis{

class KERNELSHARED_EXPORT BoundsOnlyCoordinateSystem : public CoordinateSystem
{
public:
    BoundsOnlyCoordinateSystem();
    BoundsOnlyCoordinateSystem(const Ilwis::Resource &resource);

    virtual Coordinate coord2coord(const ICoordinateSystem& sourceCs, const Coordinate& crdSource) const;
    virtual LatLon coord2latlon(const Coordinate &crdSource) const;
    virtual Coordinate latlon2coord(const LatLon& ll) const;
    virtual bool isLatLon() const;
    virtual bool isUnknown() const;
    IlwisTypes ilwisType() const;
    QString toWKT(quint32 spaces=0) const;
};
typedef IlwisData<Ilwis::BoundsOnlyCoordinateSystem> IBoundsOnlyCoordinateSystem;
}

Q_DECLARE_METATYPE(Ilwis::IBoundsOnlyCoordinateSystem);

#endif // BOUNDSONLYCOORDINATESYSTEM_H
