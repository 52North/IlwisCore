#ifndef SIMPELGEOREFERENCE_H
#define SIMPELGEOREFERENCE_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT SimpelGeoReference : public GeoReference
{
public:
    SimpelGeoReference();
    SimpelGeoReference(const Resource& res);
    virtual Coordinate pixel2Coord(const Pixel&) const;
    virtual Point2D<double> coord2Pixel(const Coordinate& crd) const;
    virtual Box2D<double> pixel2Coord(const Box2D<qint32> &box ) const;
    virtual Box2D<qint32> coord2Pixel(const Box2D<double> &box) const;
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
