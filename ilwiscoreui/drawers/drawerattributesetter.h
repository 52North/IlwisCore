#ifndef FEATURETYPEDRAWER_H
#define FEATURETYPEDRAWER_H
#include "ilwiscoreui_global.h"

namespace Ilwis {

class Feature;
class FeatureCoverage;
typedef IlwisData<FeatureCoverage> IFeatureCoverage;
class SPFeatureI;

namespace Geodrawer {
class AttributeVisualProperties;
class ILWISCOREUISHARED_EXPORT DrawerAttributeSetter
{
public:
    DrawerAttributeSetter(const IOOptions &options=IOOptions());
    ~DrawerAttributeSetter();

    virtual FeatureDrawing setSpatialAttributes(const SPFeatureI& feature, QVector<QVector3D>& vertices, QVector<QVector3D>& normals) const;
    virtual void setColorAttributes(const AttributeVisualProperties& attr,
                            const QVariant& value,
                            quint32 startIndex,
                            quint32 count,
                            std::vector<VertexColor>& colors) const;

};
}
}

#endif // FEATURETYPEDRAWER_H
