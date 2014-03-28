#include "kernel.h"
#include "location.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/Coordinate.inl"
#include "geos/geom/CoordinateSequence.h"
#include "coordinate.h"
#include "vertexiterator.h"
#include "bresenham.h"

using namespace Ilwis;

Bresenham::Bresenham()
{
}

std::vector<Pixel> Bresenham::rasterize(const VertexIterator &iterStart, VertexIterator &iterEnd)
{
    VertexIterator iter = iterStart;
    std::vector<Pixel> result;
    Coordinate crd1, crd2 ;
    while( iter != iterEnd){
        crd1 = *iter;
        crd2 = *(++iter);
        std::vector<Pixel> line  = makePixelLine(crd1, crd2);
        std::copy(line.begin(), line.end(), std::back_inserter(result));
    }
    return result;
}

std::vector<Pixel> Bresenham::makePixelLine(Coordinate crdStart, Coordinate crdEnd) const
{
    std::vector<Pixel> result;
    bool steep = (fabs(crdEnd.y - crdStart.y) > fabs(crdEnd.x - crdStart.x));
    if(steep)
    {
        std::swap(crdStart.x, crdStart.y);
        std::swap(crdEnd.x, crdEnd.y);
    }

    if(crdStart.x > crdEnd.x)
    {
        std::swap(crdStart.x, crdEnd.x);
        std::swap(crdStart.y, crdEnd.y);
    }

    double dx = crdEnd.x - crdStart.x;
    double dy = fabs(crdEnd.y - crdStart.y);

    double error = dx / 2.0;
    int ystep = (crdStart.y < crdEnd.y) ? 1 : -1;
    int y = (int)crdStart.y;

    int maxX = (int)crdEnd.x;

    for(int x=(int)crdStart.x; x<maxX; x++)
    {
        result.push_back(steep ? Pixel(y,x) : Pixel(x,y));
        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
    return result;
}
