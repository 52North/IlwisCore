#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geometryhelper.h"
#include "../drawerinterface.h"
#include "ilwistesselator.h"

void* stdAlloc(void* userData, unsigned int size)
{
    int* allocated = ( int*)userData;
    TESS_NOTUSED(userData);
    *allocated += (int)size;
    return malloc(size);
}

void stdFree(void* userData, void* ptr)
{
    TESS_NOTUSED(userData);
    free(ptr);
}

using namespace Ilwis;
using namespace Geodrawer;


IlwisTesselator::IlwisTesselator()
{
    int allocated = 0;

    memset(&_ma, 0, sizeof(_ma));
    _ma.memalloc = stdAlloc;
    _ma.memfree = stdFree;
    _ma.userData = (void*)&allocated;
    _ma.extraVertices = 256; // realloc not provided, allow 256 extra vertices.

    _tessaltor = tessNewTess(&_ma);
}

IlwisTesselator::~IlwisTesselator()
{
    tessDeleteTess(_tessaltor);
}

void IlwisTesselator::tesselate(const ICoordinateSystem &csyRoot, const ICoordinateSystem &csyGeom, const geos::geom::Geometry *geometry, std::vector<VertexPosition> &points, std::vector<VertexIndex> &indices)
{
    tessReinitialize(_tessaltor);
    std::vector<std::vector<float> > contours = getContours(geometry, csyRoot, csyGeom);
    tesselateInternal(contours,points, indices);
}

std::vector<std::vector<float> > IlwisTesselator::getContours(const geos::geom::Geometry *geometry,const ICoordinateSystem &csyRoot, const ICoordinateSystem &csyGeom)
{


    bool conversionNeeded = csyRoot != csyGeom;
    const geos::geom::Polygon *polygon = dynamic_cast<const geos::geom::Polygon *>(geometry);
    const geos::geom::LineString *outerRing = polygon->getExteriorRing();
    std::vector<std::vector<float>> contours(1);
    contours[0].resize(outerRing->getNumPoints() * 2);

    auto addCoord = [&](const geos::geom::Coordinate& crd, int index, int i) {
        if ( conversionNeeded){
            Coordinate crdTransformed = csyRoot->coord2coord(csyGeom,crd);
            contours[index][i] = crdTransformed.x;
            contours[index][i+1] = crdTransformed.y;
        } else{
           contours[index][i] = crd.x;
           contours[index][i+1] = crd.y;
        }
    };

    for(int i = 0 ; i < outerRing->getNumPoints(); ++i){
        const geos::geom::Coordinate& crd = outerRing->getCoordinateN(i);
        addCoord(crd, 0, i * 2);
    }
    if ( polygon->getNumInteriorRing() > 0){
        contours.resize(polygon->getNumInteriorRing() + 1);
        for(int i = 0; i < polygon->getNumInteriorRing(); ++i){
            const geos::geom::LineString *innerRing = polygon->getInteriorRingN(i);
            contours[i].resize(innerRing->getNumPoints() * 2);
            for(int j = 0 ; j < innerRing->getNumPoints(); ++j){
                const geos::geom::Coordinate& crd = innerRing->getCoordinateN(j);
                addCoord(crd, i+1, j * 2);
            }
        }
    }
    return contours;
}

void IlwisTesselator::tesselateInternal(const std::vector<std::vector<float> > &contours, std::vector<VertexPosition> &points, std::vector<VertexIndex> &indices)
{

    quint32 maxVerts = 0;
    for(int i = 0; i < contours.size(); ++i){
        int nverts = contours[i].size() / 2; // 2D coords
        tessAddContour(_tessaltor,2,contours[i].data(), sizeof(float) * 2, nverts);
        maxVerts += nverts;

    }
    if (!tessTesselate(_tessaltor, TESS_WINDING_ODD, TESS_POLYGONS, maxVerts, 2, 0))
        return;

    const float* verts = tessGetVertices(_tessaltor);
    const int* elems = tessGetElements(_tessaltor);
    const int nelems = tessGetElementCount(_tessaltor);

    for (int i = 0; i < nelems; ++i)
    {
        const int* p = &elems[i*maxVerts];
        quint32 oldend = points.size();
        for (int j = 0; j < maxVerts && p[j] != TESS_UNDEF; ++j){
            VertexPosition pos(verts[p[j]*2], verts[p[j]*2+1],verts[p[j]*2+2]);
            points.push_back(pos);
        }
        indices.push_back(VertexIndex(oldend,points.size() - oldend, itPOLYGON));
    }

}
