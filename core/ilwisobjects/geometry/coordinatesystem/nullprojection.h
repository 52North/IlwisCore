#ifndef NULLPROJECTION_H
#define NULLPROJECTION_H

namespace Ilwis {
class KERNELSHARED_EXPORT NullProjection : public Projection
{
public:
    NullProjection(const Ilwis::Resource &resource);
    Coordinate latlon2coord(const LatLon &ll) const;
    LatLon coord2latlon(const Coordinate &crd) const;
};
}

#endif // NULLPROJECTION_H
