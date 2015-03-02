#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "drawerinterface.h"
#include "drawerattributesetter.h"
#include "attributevisualproperties.h"

using namespace Ilwis;
using namespace Geodrawer;

DrawerAttributeSetter::DrawerAttributeSetter(const IOOptions &options)
{

}

DrawerAttributeSetter::~DrawerAttributeSetter()
{

}

FeatureDrawing DrawerAttributeSetter::setSpatialAttributes(const SPFeatureI &, QVector<QVector3D> &, QVector<QVector3D> &) const
{
    return  FeatureDrawing(itUNKNOWN);
}

void DrawerAttributeSetter::setColorAttributes(const AttributeVisualProperties &,
                                               const QVariant &,
                                               quint32 ,
                                               quint32 ,
                                               std::vector<VertexColor> &) const
{

}


