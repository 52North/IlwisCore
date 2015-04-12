#ifndef RASTERLAYERDRAWER_H
#define RASTERLAYERDRAWER_H

#include "layerdrawer.h"

class RasterImage;

namespace Ilwis {
namespace Geodrawer{
class RasterLayerDrawer : public LayerDrawer
{
public:
    RasterLayerDrawer(DrawerInterface* parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);
    ~RasterLayerDrawer();
    bool prepare(PreparationType prepType, const IOOptions& options);

    void setActiveVisualAttribute(const QString& attr);
    void coverage(const ICoverage &cov);
    ICoverage coverage() const;
    DrawerType drawerType()  const;

    static DrawerInterface *create(DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);


    NEW_DRAWER
    void setAttribute(const QString &attrName, const QVariant &value);
    QVariant attribute(const QString &attrName) const;

private:
    bool draw(const IOOptions &options);
    GLuint _texcoordid = iUNDEF;
    GLuint _textureid = iUNDEF;
    std::unique_ptr<QOpenGLTexture> _texture;
    QVector<QVector3D> _vertices;
    QVector<QVector2D> _texcoords;
    std::unique_ptr<RasterImage> _rasterImage;
    std::vector<QRgb> _colorTable;
    VisualAttribute _visualAttribute;

};
}
}

#endif // RASTERLAYERDRAWER_H
