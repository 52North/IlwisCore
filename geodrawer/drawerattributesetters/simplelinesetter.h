#ifndef SIMPLELINESETTER_H
#define SIMPLELINESETTER_H

#include "drawers/drawerinterface.h"
#include "basespatialattributesetter.h"

namespace Ilwis {
namespace Geodrawer{

class SimpleLineSetter : public BaseSpatialAttributeSetter
{
public:
    SimpleLineSetter(const IOOptions &options);
    ~SimpleLineSetter();

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
};
}
}

#endif // SIMPLELINESETTER_H
