#ifndef OPENGLHELPER_H
#define OPENGLHELPER_H

namespace geos{
namespace geom{
    class Geometry;
}
}

namespace Ilwis{
typedef std::unique_ptr<geos::geom::Geometry> UPGeometry;
class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;

namespace Geodrawer {


class OpenGLHelper
{
public:
    OpenGLHelper();

    static void getVertices(const ICoordinateSystem& csyRoot, const ICoordinateSystem& csyGeom, const UPGeometry& geometry, std::vector<VertexPosition>& points,  std::vector<VertexIndex>& indices);
private:
    static void getPolygonVertices(const ICoordinateSystem& csyRoot, const ICoordinateSystem& csyGeom, const Ilwis::UPGeometry &geometry, std::vector<VertexPosition> &points, std::vector<VertexIndex> &indices);
    static void getLineVertices(const ICoordinateSystem &csyRoot, const ICoordinateSystem& csyGeom, const Ilwis::UPGeometry &geometry, std::vector<VertexPosition> &points, std::vector<VertexIndex> &indices);
    static void getPointVertices(const ICoordinateSystem& csyRoot, const ICoordinateSystem& csyGeom, const UPGeometry &geometry, std::vector<VertexPosition> &points, std::vector<VertexIndex> &indices);
};
}
}

#endif // OPENGLHELPER_H
