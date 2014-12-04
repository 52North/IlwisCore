#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/CoordinateSequence.h"
#include "drawerinterface.h"
#include "geometryhelper.h"
#include "openglhelper.h"

using namespace Ilwis;
using namespace Geodrawer;

OpenGLHelper::OpenGLHelper()
{
}


void OpenGLHelper::getVertices(const ICoordinateSystem& csyRoot, const ICoordinateSystem& csyGeom, const Ilwis::UPGeometry &geometry, std::vector<VertexPosition> &points, std::vector<VertexIndex> &indices)
{
    IlwisTypes tp =  GeometryHelper::geometryType(geometry.get());
    switch( tp)     {
        case itPOLYGON:
            getPolygonVertices(csyRoot, csyGeom, geometry, points, indices);break;
        case itLINE:
            getLineVertices(csyRoot, csyGeom,geometry, points, indices); break;
        case itPOINT:
            getPointVertices(csyRoot, csyGeom,geometry, points, indices); break;\
        default:
            break;
    }
}

void OpenGLHelper::getPolygonVertices(const ICoordinateSystem& csyRoot, const ICoordinateSystem& csyGeom,const Ilwis::UPGeometry &geometry, std::vector<VertexPosition> &points, std::vector<VertexIndex> &indices){

}

void OpenGLHelper::getLineVertices(const ICoordinateSystem& csyRoot, const ICoordinateSystem& csyGeom, const Ilwis::UPGeometry &geometry, std::vector<VertexPosition> &points, std::vector<VertexIndex> &indices){

    auto *coords = geometry->getCoordinates();
    quint32 oldend = points.size();
    indices.push_back(VertexIndex(oldend, coords->size(), itLINE));
    points.resize(oldend + coords->size());
    bool conversionNeeded = csyRoot != csyGeom;
    Coordinate crd;
    for(int i = 0; i < coords->size(); ++i){
        coords->getAt(i, crd);
        if ( conversionNeeded){
            crd = csyRoot->coord2coord(csyGeom, crd);
        }
        points[oldend + i] = VertexPosition(crd.x, crd.y, crd.z);
    }
    delete coords;

}

void OpenGLHelper::getPointVertices(const ICoordinateSystem& csyRoot, const ICoordinateSystem& csyGeom, const Ilwis::UPGeometry &geometry, std::vector<VertexPosition> &points, std::vector<VertexIndex> &indices){

}
