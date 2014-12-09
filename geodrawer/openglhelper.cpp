#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/CoordinateSequence.h"
#include "drawerinterface.h"
#include "geometryhelper.h"
#include "tesselation/ilwistesselator.h"
#include "openglhelper.h"
#include "tesselation/tesselator.h"

using namespace Ilwis;
using namespace Geodrawer;

IlwisTesselator OpenGLHelper::_tesselator;

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
    int n = geometry->getNumGeometries();
    for(int  geom = 0; geom < n; ++geom ){
        const geos::geom::Geometry *subgeom = geometry->getGeometryN(geom);
        if (!subgeom)
            continue;
        _tesselator.tesselate(csyRoot,csyGeom, subgeom,points, indices);
    }
}

void OpenGLHelper::getLineVertices(const ICoordinateSystem& csyRoot, const ICoordinateSystem& csyGeom, const Ilwis::UPGeometry &geometry, std::vector<VertexPosition> &points, std::vector<VertexIndex> &indices){

    int n = geometry->getNumGeometries();
    for(int  geom = 0; geom < n; ++geom ){
        const geos::geom::Geometry *subgeom = geometry->getGeometryN(geom);
        if (!subgeom)
            continue;
        auto *coords = subgeom->getCoordinates();
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

}

void OpenGLHelper::getPointVertices(const ICoordinateSystem& csyRoot, const ICoordinateSystem& csyGeom, const Ilwis::UPGeometry &geometry, std::vector<VertexPosition> &points, std::vector<VertexIndex> &indices){

}
