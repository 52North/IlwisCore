#include <QVector3D>
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "geometryhelper.h"
#include "drawers/attributevisualproperties.h"
#include "drawers/drawerattributesetterfactory.h"
#include "simplelinesetter.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_DRAWER_ATTRIBUTE_SETTER(SimpleLineSetter)

SimpleLineSetter::SimpleLineSetter(const IOOptions &options) : BaseSpatialAttributeSetter(options)
{

}

SimpleLineSetter::~SimpleLineSetter()
{

}

DrawerAttributeSetter *SimpleLineSetter::create(const IOOptions &options)
{
    return new SimpleLineSetter(options);

}

std::vector<VertexIndex> SimpleLineSetter::setSpatialAttributes(const Ilwis::SPFeatureI &feature,
                                               QVector<QVector3D> &vertices,
                                               QVector<QVector3D> &) const
{
    const UPGeometry& geometry = feature->geometry();
    std::vector<VertexIndex> indices;
    int n = geometry->getNumGeometries();
    for(int  geom = 0; geom < n; ++geom ){
        const geos::geom::Geometry *subgeom = geometry->getGeometryN(geom);
        if (!subgeom)
            continue;
        auto *coords = subgeom->getCoordinates();
        quint32 oldend = vertices.size();
        indices.push_back(VertexIndex(oldend, coords->size(), itLINE, GL_LINE_STRIP, feature->featureid()));
        vertices.resize(oldend + coords->size());

        Coordinate crd;
        for(int i = 0; i < coords->size(); ++i){
            coords->getAt(i, crd);
            if ( coordinateConversionNeeded()){
                crd = _targetSystem->coord2coord(_sourceSystem, crd);
            }
            vertices[oldend + i] = QVector3D(crd.x, crd.y, crd.z);
        }
        delete coords;
    }
    return indices;
}

void SimpleLineSetter::setColorAttributes(const AttributeVisualProperties &attr, const QVariant &value, quint32 startIndex, quint32 count, std::vector<VertexColor> &colors) const
{
    if ( value.isValid() && value.toInt() != iUNDEF) {
        for(int i =startIndex; i < startIndex + count; ++i){
            QColor clr = attr.value2color(value);
            colors.push_back(VertexColor(clr.redF(), clr.greenF(), clr.blueF(), 1.0));
        }
    }
}

