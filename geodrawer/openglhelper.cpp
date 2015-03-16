#include <QVector3D>
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/CoordinateSequence.h"
#include "drawers/drawerinterface.h"
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


quint32 OpenGLHelper::getVertices(const ICoordinateSystem& csyRoot,
                               const ICoordinateSystem& csyGeom,
                               const Ilwis::UPGeometry &geometry,
                               Raw objectid,
                               QVector<QVector3D> &points,
                               QVector<QVector3D>& normals,
                               std::vector<VertexIndex> &indices,
                               quint32& boundaryIndex)
{
    quint32 oldNumberOfVertices = points.size();
    IlwisTypes tp =  GeometryHelper::geometryType(geometry.get());

       switch( tp)     {
        case itPOLYGON:
            getPolygonVertices(csyRoot, csyGeom, geometry, objectid, points, normals, indices);
        case itLINE:
            getLineVertices(csyRoot, csyGeom,geometry, objectid, points, normals, indices); break;
        case itPOINT:
            getPointVertices(csyRoot, csyGeom,geometry, objectid, points, normals, indices); break;\
        default:
            break;
    }
    return points.size() - oldNumberOfVertices;

}

void OpenGLHelper::getPolygonVertices(const ICoordinateSystem& csyRoot,
                                      const ICoordinateSystem& csyGeom,
                                      const Ilwis::UPGeometry &geometry,
                                      Raw objectid,
                                      QVector<QVector3D> &points,
                                      QVector<QVector3D>& normals,
                                      std::vector<VertexIndex> &indices){
    int n = geometry->getNumGeometries();
    for(int  geom = 0; geom < n; ++geom ){
        const geos::geom::Geometry *subgeom = geometry->getGeometryN(geom);
        if (!subgeom)
            continue;
        _tesselator.tesselate(csyRoot,csyGeom, subgeom,objectid, points, indices);
    }
}

void OpenGLHelper::getLineVertices(const ICoordinateSystem& csyRoot,
                                   const ICoordinateSystem& csyGeom,
                                   const Ilwis::UPGeometry &geometry,
                                   Raw objectid,
                                   QVector<QVector3D> &points,
                                   QVector<QVector3D> &normals,
                                   std::vector<VertexIndex> &indices){

    int n = geometry->getNumGeometries();
    for(int  geom = 0; geom < n; ++geom ){
        const geos::geom::Geometry *subgeom = geometry->getGeometryN(geom);
        if (!subgeom)
            continue;
        auto *coords = subgeom->getCoordinates();
        quint32 oldend = points.size();
        indices.push_back(VertexIndex(oldend, coords->size(), GL_LINE_STRIP, objectid));
        points.resize(oldend + coords->size());
        bool conversionNeeded = csyRoot != csyGeom;
        Coordinate crd;
        for(int i = 0; i < coords->size(); ++i){
            coords->getAt(i, crd);
            if ( conversionNeeded){
                crd = csyRoot->coord2coord(csyGeom, crd);
            }
            points[oldend + i] = QVector3D(crd.x, crd.y, crd.z);
        }
        delete coords;
    }

}

void OpenGLHelper::getPointVertices(const ICoordinateSystem& csyRoot,
                                    const ICoordinateSystem& csyGeom,
                                    const Ilwis::UPGeometry &geometry,
                                    Raw objectid, QVector<QVector3D> &points,
                                    QVector<QVector3D> &normals,
                                    std::vector<VertexIndex> &indices){

    int n = geometry->getNumGeometries();
    for(int  geom = 0; geom < n; ++geom ){
        const geos::geom::Geometry *subgeom = geometry->getGeometryN(geom);
        if (!subgeom)
            continue;
        const geos::geom::Coordinate *crd = subgeom->getCoordinate();
        indices.push_back(VertexIndex(points.size(),1,GL_POINTS,objectid));
        bool conversionNeeded = csyRoot != csyGeom;
        Coordinate coord = *crd;
        if ( conversionNeeded){
            coord = csyRoot->coord2coord(csyGeom, *crd);
        }
        points.push_back(QVector3D(coord.x, coord.y, coord.z));
    }
}
