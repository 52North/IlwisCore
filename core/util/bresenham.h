#ifndef BRESENHAM_H
#define BRESENHAM_H

namespace Ilwis {
class Bresenham
{
public:
    Bresenham();
    std::vector<Pixel> rasterize(const VertexIterator& iterStart, VertexIterator& iterEnd);

private:
    std::vector<Pixel> makePixelLine(Ilwis::Coordinate crdStart, Ilwis::Coordinate crdEnd) const;
};
}

#endif // BRESENHAM_H
