#ifndef SIMPLEPOINTSETTER_H
#define SIMPLEPOINTSETTER_H

#include "drawers/drawerinterface.h"
#include "basespatialattributesetter.h"

namespace Ilwis {
namespace Geodrawer{
class SimplePointSetter : public BaseSpatialAttributeSetter
{
public:
    SimplePointSetter(const IOOptions &options);
    ~SimplePointSetter();

    static DrawerAttributeSetter *create(const IOOptions& options);

    FeatureDrawing setSpatialAttributes(const SPFeatureI& feature,
                                 QVector<QVector3D>& vertices,
                                 QVector<QVector3D>& normals) const;

    void setColorAttributes(const AttributeVisualProperties& attr,
                            const QVariant& value,
                            quint32 startIndex,
                            quint32 count,
                            std::vector<VertexColor>& colors) const;

    NEW_DRAWER_ATTRIBUTE_SETTER
};
}
}

#endif // SIMPLEPOINTSETTER_H
