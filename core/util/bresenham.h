#ifndef BRESENHAM_H
#define BRESENHAM_H

namespace Ilwis {

class VertexIterator;

class Bresenham
{
public:
    Bresenham(const IGeoReference &grf);
    std::vector<Pixel> rasterize(const VertexIterator& iterStart, const VertexIterator& iterEnd);

private:
    std::vector<Pixel> makePixelLine(const Ilwis::Coordinate &crdStart, const Ilwis::Coordinate &crdEnd, bool &valid, const Size <> & sz, quint32 subcount) const;
    bool inBounds(Pixel cur, const Size <> & sz) const;
    IGeoReference _targetGrf;
    bool _valid = false;
};
}

#endif // BRESENHAM_H
