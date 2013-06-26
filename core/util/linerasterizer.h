#ifndef LINERASTERIZER_H
#define LINERASTERIZER_H

namespace Ilwis {
class KERNELSHARED_EXPORT LineRasterizer
{
public:
    LineRasterizer(const IGeoReference &grf, const ICoordinateSystem &csyIn);

    std::vector<Pixel> rasterize(const Coordinate2d &start, const Coordinate2d &end) const;

private:
    IGeoReference _grf;
    ICoordinateSystem _csy;

    bool inBounds(const Pixel& cur, const QSize &size) const;
};
}

#endif // LINERASTERIZER_H
