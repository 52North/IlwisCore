#ifndef LINERASTERIZER_H
#define LINERASTERIZER_H

namespace Ilwis {
class KERNELSHARED_EXPORT LineRasterizer
{
public:
    LineRasterizer(const IGeoReference &grf, const ICoordinateSystem &csyIn);

    std::vector<Pixel> rasterize(const Coordinate &start, const Coordinate &end) const;

private:
    IGeoReference _grf;
    ICoordinateSystem _csy;

    bool inBounds(const Ilwis::Pixeld &cur, const Ilwis::Size<> &size) const;
};
}

#endif // LINERASTERIZER_H
