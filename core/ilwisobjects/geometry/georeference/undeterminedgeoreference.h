#ifndef UNDETERMINEDGEOREFERENCE_H
#define UNDETERMINEDGEOREFERENCE_H

namespace Ilwis {
class UndeterminedGeoReference : public GeoRefImplementation
{
public:
    UndeterminedGeoReference();

    virtual Coordinate pixel2Coord(const Pixeld&pix) const ;
    virtual Pixeld coord2Pixel(const Coordinate& crd) const;
    virtual double pixelSize() const;
    static GeoRefImplementation *create();
};
}

#endif // UNDETERMINEDGEOREFERENCE_H
