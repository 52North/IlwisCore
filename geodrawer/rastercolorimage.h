#ifndef RASTERCOLORIMAGE_H
#define RASTERCOLORIMAGE_H

#include "drawers/rasterimage.h"


namespace Ilwis{
namespace Geodrawer {

class RGBTexture;

class RasterColorImage : public RasterImage
{
public:
    RasterColorImage(DrawerInterface *rootDrawer, const IRasterCoverage &raster, const VisualAttribute &vattribute, const IOOptions &options);
    ~RasterColorImage();
    bool prepare(int prepareType);

protected:
    void setTextureData(RGBTexture * tex, const unsigned int offsetX, const unsigned int offsetY, unsigned int texSizeX, unsigned int texSizeY, unsigned int zoomFactor);
    virtual Texture * GenerateTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor);

private:
    static     RasterImage *create(DrawerInterface *rootDrawer, const IRasterCoverage &raster, const VisualAttribute &vattribute, const IOOptions &options);

    NEW_RASTERIMAGETYPE
};

class RGBTexture : public Texture
{
public:
    RGBTexture(const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor);
    virtual ~RGBTexture();
    void setTexture(QOpenGLTexture * texture);

protected:
};

}
}

#endif // RASTERCOLORIMAGE_H
