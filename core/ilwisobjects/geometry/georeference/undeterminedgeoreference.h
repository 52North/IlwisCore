#ifndef UNDETERMINEDGEOREFERENCE_H
#define UNDETERMINEDGEOREFERENCE_H

namespace Ilwis {

class KERNELSHARED_EXPORT UndeterminedGeoReference : public GeoRefImplementation
{
public:
    UndeterminedGeoReference();

    virtual Coordinate pixel2Coord(const Pixeld&pix) const ;
    virtual Pixeld coord2Pixel(const Coordinate& crd) const;
    virtual double pixelSize() const;
    GeoRefImplementation *clone();

    static GeoRefImplementation *create();
    static QString typeName();
};
}

#endif // UNDETERMINEDGEOREFERENCE_H
