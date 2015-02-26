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

    std::vector<VertexIndex> setSpatialAttributes(const SPFeatureI& feature,
                                 QVector<QVector3D>& vertices,
                                 QVector<QVector3D>& normals) const;

    void setColorAttributes(const AttributeVisualProperties& attr,
                            const QVariant& value,
                            quint32 startIndex,
                            quint32 count,
                            std::vector<VertexColor>& colors) const;

    NEW_DRAWER_ATTRIBUTE_SETTER

private:
    IlwisTesselator _tesselator;
};
}
}

#endif // SIMPLEPOLYGONSETTER_H
