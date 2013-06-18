#ifndef GEOREFERENCE_H
#define GEOREFERENCE_H

#include "Kernel_global.h"
#include "georefadapter.h"

namespace Ilwis {

class KERNELSHARED_EXPORT GeoReference : public IlwisObject
{
public:
    GeoReference();
    GeoReference(const Resource& res);

    virtual Coordinate pixel2Coord(const Pixel_d&) const = 0;
    virtual Pixel_d coord2Pixel(const Coordinate& crd) const = 0;
    virtual Box2D<double> pixel2Coord(const Box2D<qint32>& box ) const;
    virtual Box2D<qint32> coord2Pixel(const Box2D<double>& box) const;
    ICoordinateSystem coordinateSystem() const;
    void coordinateSystem(const ICoordinateSystem& csy);
    virtual double pixelSize() const = 0;
    virtual bool compute() = 0;
    Size size() const;
    void size(const Size& sz);
    bool centerOfPixel() const;
    void centerOfPixel(bool yesno);
    void adapter(GeoRefAdapter* adapt);
protected:
    const QScopedPointer<GeoRefAdapter> &adapter() const;
private:
    ICoordinateSystem _csy;
    Size _size;
    bool _centerOfPixel;
    QScopedPointer<GeoRefAdapter> _adapter;
};

typedef IlwisData<GeoReference> IGeoReference;
}

Q_DECLARE_METATYPE(Ilwis::IGeoReference)

#endif // GEOREFERENCE_H
