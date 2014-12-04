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
    static DrawerInterface *create(DrawerInterface *parentDrawer, RootDrawer *rootdrawer);

    NEW_DRAWER

private:
    std::vector<VertexIndex> _indices;

    bool draw(QOpenGLContext *openglContext, const IOOptions &options);
};
}
}

#endif // FEATURELAYERDRAWER_H
