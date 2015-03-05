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


private:
    std::vector<VertexIndex> _indices;
    QVector<QVector3D> _vertices;
    QVector<QVector3D> _normals;
    std::vector<VertexColor> _colors;
    quint32 _boundaryIndex = iUNDEF;

    bool draw(const IOOptions &options);


};
}
}

#endif // FEATURELAYERDRAWER_H
