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
    void DisplayImagePortion(unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY);
    void DisplayTexture(Coordinate & c1, Coordinate & c2, Coordinate & c3, Coordinate & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor);
    GLuint _texturemat = iUNDEF;
    GLuint _texcoordid = iUNDEF;
    GLuint _textureid = iUNDEF;
    std::unique_ptr<RasterImage> _rasterImage;
    VisualAttribute _visualAttribute;
    int _maxTextureSize;
    unsigned long _imageWidth;
    unsigned long _imageHeight;
    unsigned long _width;
    unsigned long _height;

};
}
}

#endif // RASTERLAYERDRAWER_H
