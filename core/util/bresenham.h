#ifndef BRESENHAM_H
#define BRESENHAM_H
#include "kernel_global.h"

namespace Ilwis {

class VertexIterator;

class KERNELSHARED_EXPORT Bresenham
{
public:
    Bresenham(const IGeoReference &grf, const Ilwis::ICoordinateSystem &csy=ICoordinateSystem());
    std::vector<Pixel> rasterize(const VertexIterator& iterStart, const VertexIterator& iterEnd);

private:
    std::vector<Pixel> makePixelLine(const Ilwis::Coordinate &crdStart, const Ilwis::Coordinate &crdEnd, bool &valid, const Size <> & sz, quint32 subcount) const;
    bool inBounds(Pixel cur, const Size <> & sz) const;
    IGeoReference _targetGrf;
    ICoordinateSystem _sourceCsy;
    bool _valid = false;
};
}

#endif // BRESENHAM_H
