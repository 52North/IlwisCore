#ifndef SIMPELGEOREFERENCE_H
#define SIMPELGEOREFERENCE_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT SimpelGeoReference : public GeoReference
{
public:
    SimpelGeoReference();
    SimpelGeoReference(const Resource& res);
    virtual Coordinate pixel2Coord(const Pixel_d&) const;
    virtual Pixel_d coord2Pixel(const Coordinate& crd) const;
    virtual double pixelSize() const;

    std::vector<double> matrix() const;
    std::vector<double> support() const;
protected:
    void clear();
    double _a11,_a12,_a22,_a21,_b1,_b2;
    double _det;

};


}

#endif // SIMPELGEOREFERENCE_H
