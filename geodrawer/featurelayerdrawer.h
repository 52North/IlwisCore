#ifndef FEATURELAYERDRAWER_H
#define FEATURELAYERDRAWER_H

#include "layerdrawer.h"

namespace Ilwis {
namespace Geodrawer{


class FeatureLayerDrawer : public LayerDrawer
{
public:
    FeatureLayerDrawer(DrawerInterface* parentDrawer, RootDrawer *rootdrawer);
    bool prepare(PreparationType prepType, const IOOptions& options,QOpenGLContext *openglContext=0);
    void unprepare(DrawerInterface::PreparationType prepType);

    void setActiveDataAttribute(const QString& attr);
    void coverage(const ICoverage &cov);
    ICoverage coverage() const;

    static DrawerInterface *create(DrawerInterface *parentDrawer, RootDrawer *rootdrawer);


    NEW_DRAWER


private:
    std::vector<VertexIndex> _indices;
    quint32 _boundaryIndex = iUNDEF;

    bool draw(QOpenGLContext *openglContext, const IOOptions &options);
};
}
}

#endif // FEATURELAYERDRAWER_H
