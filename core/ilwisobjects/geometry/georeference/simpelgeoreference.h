#ifndef SIMPELGEOREFERENCE_H
#define SIMPELGEOREFERENCE_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT SimpelGeoReference : public GeoRefImplementation
{
public:
    SimpelGeoReference();
    SimpelGeoReference(const QString &type);
    static GeoRefImplementation * create();
     virtual Coordinate pixel2Coord(const Pixeld&) const;
    virtual Pixeld coord2Pixel(const Coordinate& crd) const;
    virtual double pixelSize() const;
    bool isCompatible(const IGeoReference &georefOther) const;

    std::vector<double> matrix() const;
    std::vector<double> support() const;
    static QString typeName();
protected:
    void clear();
    double _a11,_a12,_a22,_a21,_b1,_b2;
    double _det;

};


}

#endif // SIMPELGEOREFERENCE_H
