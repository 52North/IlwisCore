#ifndef FEATURELAYERDRAWER_H
#define FEATURELAYERDRAWER_H

#include "layerdrawer.h"

namespace Ilwis {
namespace Geodrawer{


class FeatureLayerDrawer : public LayerDrawer
{
public:
    FeatureLayerDrawer(DrawerInterface* parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);
    bool prepare(PreparationType prepType, const IOOptions& options);
    void unprepare(DrawerInterface::PreparationType prepType);

    void setActiveVisualAttribute(const QString& attr);
    void coverage(const ICoverage &cov);
    ICoverage coverage() const;
    DrawerType drawerType()  const;

    static DrawerInterface *create(DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);


    NEW_DRAWER
    void setAttribute(const QString &attrName, const QVariant &value);
    QVariant attribute(const QString &attrName) const;


private:

    QVector<QVector3D> _vertices;
    QVector<QVector3D> _normals;
    std::vector<VertexIndex> _indices;
    std::vector<VertexColor> _colors;

    std::vector<FeatureDrawing> _featureDrawings;
    float _lineWidth = 1.0;
    float _boundarywidth = 1.0;
    bool _showBoundaries = true;
    bool _showAreas = true;
    float _areaTransparency = 0;
    QColor _boundaryColor = QColor("black");

    bool draw(const IOOptions &options);


};
}
}

#endif // FEATURELAYERDRAWER_H
