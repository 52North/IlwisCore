#ifndef ILWISTESSELATOR_H
#define ILWISTESSELATOR_H

#include "tesselator.h"

namespace Ilwis {
namespace Geodrawer{

class IlwisTesselator
{
public:
    IlwisTesselator();
    ~IlwisTesselator();

    void tesselate(const ICoordinateSystem &csyRoot, const ICoordinateSystem &csyGeom, const geos::geom::Geometry *geometry, Raw objectid, std::vector<VertexPosition> &points, std::vector<VertexIndex> &indices);
private:
   std::vector<std::vector<float>> getContours(const geos::geom::Geometry *geometry,const ICoordinateSystem &csyRoot, const ICoordinateSystem &csyGeom);
   void tesselateInternal(const std::vector<std::vector<float>>& contours, Raw objectid,std::vector<VertexPosition> &points, std::vector<VertexIndex> &indices);

    TESStesselator *_tessaltor = 0;
    TESSalloc _ma;
};
}
}

#endif // ILWISTESSELATOR_H
