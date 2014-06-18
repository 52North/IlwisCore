#include "kernel.h"
#include "location.h"
#include "ilwisdata.h"
#include "size.h"
#include "geos/geom/Coordinate.h"
#include "coordinate.h"
#include "box.h"
#include "georeference.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/CoordinateSequence.h"
#include "coordinate.h"
#include "vertexiterator.h"
#include "bresenham.h"

using namespace Ilwis;

Bresenham::Bresenham(const IGeoReference& grf) : _targetGrf(grf){

}

std::vector<Pixel> Bresenham::rasterize(const VertexIterator &iterStart, const VertexIterator &iterEnd)
{
    VertexIterator iter = iterStart;
    std::vector<Pixel> result;
    Coordinate crd1, crd2 ;
    auto endIter = iterEnd - 1;
    while( iter != endIter){
        crd1 = *iter;
        crd2 = *(++iter);
        std::vector<Pixel> line  = makePixelLine(crd1, crd2, _valid);
        if (!_valid)
            return std::vector<Pixel>();
        std::copy(line.begin(), line.end(), std::back_inserter(result));
    }
    // remove duplicates
    auto iterUnique = std::unique(result.begin(), result.end());
    result.resize(std::distance(result.begin(),iterUnique));
    return result;
}

std::vector<Pixel> Bresenham::makePixelLine(const Coordinate& crdStart, const Coordinate& crdEnd, bool& valid) const
{
    valid = false;
    std::vector<Pixel> result;
    if ( !_targetGrf.isValid()){
        ERROR2(ERR_NO_INITIALIZED_2,TR("target georeference"), "bresenham algorithm");
        return result;
    }

    Pixeld pixStart = _targetGrf->coord2Pixel(crdStart);
    Pixeld pixEnd = _targetGrf->coord2Pixel(crdEnd);
    if ( !pixStart.isValid() || !pixEnd.isValid()){
        ERROR2(ERR_INVALID_INIT_FOR_2, TR("coordinates"), "bresenham algorithm");
        return result;
    }
    valid = true;
    bool steep = (fabs(pixEnd.y - pixStart.y) > fabs(pixEnd.x - pixStart.x));
    if(steep)
    {
        std::swap(pixStart.x, pixStart.y);
        std::swap(pixEnd.x, pixEnd.y);
    }

    if(pixStart.x > pixEnd.x)
    {
        std::swap(pixStart.x, pixEnd.x);
        std::swap(pixStart.y, pixEnd.y);
    }

    double dx = pixEnd.x - pixStart.x;
    double dy = fabs(pixEnd.y - pixStart.y);

    double error = dx / 2.0;
    int ystep = (pixStart.y < pixEnd.y) ? 1 : -1;
    int y = (int)pixStart.y;

    int maxX = (int)pixEnd.x;

    for(int x=(int)pixStart.x; x<maxX; x++)
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
