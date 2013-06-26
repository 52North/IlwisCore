#include "kernel.h"
#include "ilwis.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "coordinatesystem.h"
#include "linerasterizer.h"

using namespace Ilwis;

LineRasterizer::LineRasterizer(const IGeoReference& grf, const ICoordinateSystem& csyIn) : _grf(grf), _csy(csyIn)
{
}

std::vector<Pixel> LineRasterizer::rasterize(const Coordinate2d &start, const Coordinate2d &end) const
{
    std::vector<Pixel> result;
    Coordinate2d c1 = start;
    Coordinate2d c2 = end;
    if ( _grf->coordinateSystem() != _csy) {
        c1 = _grf->coordinateSystem()->coord2coord(_csy, c1);
        c2 = _grf->coordinateSystem()->coord2coord(_csy, c2);
    }
    if ( !c1.isValid() || !c2.isValid()) {
        ERROR2(ERR_NO_INITIALIZED_2,"Coordinates", "Line rasterization");
        return result;
    }

    Pixel p1 = _grf->coord2Pixel(c1);
    Pixel p2 = _grf->coord2Pixel(c2);
    Box2D<> bounds(p1,p2);

    if ( p1.x() < 0 && p2.x() < 0)
        return result;
    if ( p1.y() < 0 && p2.y() < 0)
        return result;
    if ( (p1.x() - p2.x()) == 0) {   // angle = 90
        if ( p2.y() < p1.y()) {
            Pixel temp = p2;
            p2 = p1;
            p1 = temp;
        }
        for(double y=0; y < p2.y() - p1.y(); ++y) {
            Pixel cur((double)p1.x(),(double)p1.y() + y);

            if ( inBounds(cur, bounds.size()))
                result.push_back(cur);
        }
    }
    else {
        double tan = (double)(p1.y() - p2.y()) / ( p1.x() - p2.x());
        bool dir = p2.y() < p1.y();
        if  (dir){
            Pixel temp = p2;
            p2 = p1;
            p1 = temp;
        }

        Pixel prev;
        Pixel cur;
        if ( abs(tan) <= 1) {
            double xdir = p2.x() - p1.x() > 0 ? 1 : -1;
            for(double x=0; x <= abs(p2.x() - p1.x()); ++x) {
                cur = Pixel( (double)p1.x() + xdir * x,(double)p1.y() + tan * x  * xdir );\
                if ( cur == prev)
                    continue;
                if ( inBounds(cur, bounds.size()))
                    result.push_back(cur);
                prev= cur;
            }

        } else {
            double ydir = p2.y() - p1.y() > 0 ? 1 : -1;
            for(double y=0; y <= abs(p2.y() - p1.y()); ++y) {
                cur = Pixel( (double)p1.x() + 1.0/tan * ydir * y,(double)p1.y() + y);
                if ( cur == prev)
                    continue;
                if ( inBounds(cur, bounds.size()))
                    result.push_back(cur);
                prev = cur;
            }
        }
    }
    return result;
}

bool LineRasterizer::inBounds(const Pixel& cur, const QSize& size) const{
    return !( cur.x() < 0 || cur.y() < 0 || cur.x() >= size.width() || cur.y() >= size.height());
}
