#include <QVector3D>
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "geometryhelper.h"
#include "drawers/attributevisualproperties.h"
#include "drawers/drawerattributesetterfactory.h"
#include "simplepointsetter.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_DRAWER_ATTRIBUTE_SETTER(SimplePointSetter)

Ilwis::Geodrawer::SimplePointSetter::SimplePointSetter(const Ilwis::IOOptions &options) : BaseSpatialAttributeSetter(options)
{

}


SimplePointSetter::~SimplePointSetter()
{

}

DrawerAttributeSetter *SimplePointSetter::create(const IOOptions &options)
{
    return new SimplePointSetter(options)  ;
}

std::vector<VertexIndex> SimplePointSetter::setSpatialAttributes(const SPFeatureI &feature, QVector<QVector3D> &vertices, QVector<QVector3D> &) const
{
    Envelope env = _rootDrawer->attribute("coverageenvelope").value<Envelope>();
    double size = env.xlength() / 50.0;
    const UPGeometry& geometry = feature->geometry();
    int n = geometry->getNumGeometries();
    std::vector<VertexIndex> indices;
    for(int  geom = 0; geom < n; ++geom ){
        const geos::geom::Geometry *subgeom = geometry->getGeometryN(geom);
        if (!subgeom)
            continue;
        const geos::geom::Coordinate *crd = subgeom->getCoordinate();
        Coordinate coord = *crd;
        if ( coordinateConversionNeeded()){
            coord = _targetSystem->coord2coord(_sourceSystem, *crd);
        }

        indices.push_back(VertexIndex(vertices.size(),5,itPOINT,GL_LINE_STRIP,feature->featureid()));
        vertices.push_back(QVector3D(coord.x + size, coord.y + size, coord.z));
        vertices.push_back(QVector3D(coord.x - size, coord.y + size, coord.z));
        vertices.push_back(QVector3D(coord.x - size, coord.y - size, coord.z));
        vertices.push_back(QVector3D(coord.x + size, coord.y - size, coord.z));
        vertices.push_back(QVector3D(coord.x + size, coord.y + size, coord.z));
    }
    return indices;

}

void SimplePointSetter::setColorAttributes(const AttributeVisualProperties &attr, const QVariant &value, quint32 startIndex, quint32 count, std::vector<VertexColor> &colors) const
{
    if ( value.isValid() && value.toInt() != iUNDEF) {
        for(int i =startIndex; i < startIndex + count; ++i){
            QColor clr = attr.value2color(value);
            colors.push_back(VertexColor(clr.redF(), clr.greenF(), clr.blueF(), 1.0));
        }
    }
}

