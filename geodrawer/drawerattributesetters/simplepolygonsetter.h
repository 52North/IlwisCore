#ifndef SIMPLEPOLYGONSETTER_H
#define SIMPLEPOLYGONSETTER_H

#include "drawers/drawerinterface.h"
#include "basespatialattributesetter.h"

namespace Ilwis {
namespace Geodrawer{


class SimplePolygonSetter : public BaseSpatialAttributeSetter
{
public:
    SimplePolygonSetter(const IOOptions &options);
    ~SimplePolygonSetter();

    static DrawerAttributeSetter *create(const IOOptions& options);

    FeatureDrawing setSpatialAttributes(const SPFeatureI& feature,
                                 QVector<QVector3D>& vertices,
                                 QVector<QVector3D>& normals) const;

    void setColorAttributes(const VisualAttribute& attr,
                            const QVariant& value,const QColor& defaultColor,
                            const FeatureDrawing& drawing,
                            std::vector<VertexColor>& colors) const;

    NEW_DRAWER_ATTRIBUTE_SETTER

private:
    IlwisTesselator _tesselator;
};
}
}

#endif // SIMPLEPOLYGONSETTER_H
