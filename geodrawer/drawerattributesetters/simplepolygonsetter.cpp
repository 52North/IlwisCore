#include <QVector3D>
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "geometryhelper.h"
#include "drawers/drawerinterface.h"
#include "drawers/attributevisualproperties.h"
#include "drawers/drawerattributesetterfactory.h"
#include "../tesselation/ilwistesselator.h"
#include "simplepolygonsetter.h"


using namespace Ilwis;
using namespace Geodrawer;

REGISTER_DRAWER_ATTRIBUTE_SETTER(SimplePolygonSetter)

SimplePolygonSetter::SimplePolygonSetter(const IOOptions &options) : BaseSpatialAttributeSetter(options)
{

}

SimplePolygonSetter::~SimplePolygonSetter()
{

}

DrawerAttributeSetter *SimplePolygonSetter::create(const Ilwis::IOOptions &options)
{
    return new SimplePolygonSetter(options)    ;
}

FeatureDrawing SimplePolygonSetter::setSpatialAttributes(const SPFeatureI &feature, QVector<QVector3D> &vertices, QVector<QVector3D> &normals) const
{
    IlwisTesselator tesselator;
    const UPGeometry& geometry = feature->geometry();
    int n = geometry->getNumGeometries();
    FeatureDrawing drawing(itPOLYGON);
    for(int  geom = 0; geom < n; ++geom ){
        const geos::geom::Geometry *subgeom = geometry->getGeometryN(geom);
        if (!subgeom)
            continue;
        tesselator.tesselate(_targetSystem,_sourceSystem, subgeom,feature->featureid(), vertices, drawing._indices);
    }
    return drawing;
}

void SimplePolygonSetter::setColorAttributes(const VisualAttribute &attr, const QVariant &value, quint32 startIndex, quint32 count, std::vector<Ilwis::Geodrawer::VertexColor> &colors) const
{
    if ( value.isValid() && value.toInt() != iUNDEF) {
        for(int i =startIndex; i < startIndex + count; ++i){
            QColor clr = attr.value2color(value);
            colors.push_back(VertexColor(clr.redF(), clr.greenF(), clr.blueF(), 1.0));
        }
    }
}
