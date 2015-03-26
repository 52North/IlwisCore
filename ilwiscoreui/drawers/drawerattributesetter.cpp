#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "drawerinterface.h"
#include "attributevisualproperties.h"
#include "drawerattributesetter.h"


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

void DrawerAttributeSetter::setColorAttributes(const VisualAttribute &,
                                               const QVariant &,
                                               const FeatureDrawing &,
                                               std::vector<VertexColor> &) const
{

}


