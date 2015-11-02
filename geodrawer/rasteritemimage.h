#ifndef RASTERITEMIMAGE_H
#define RASTERITEMIMAGE_H


#include "drawers/rasterimage.h"

namespace Ilwis{
namespace Geodrawer {

class ItemTexture;

class RasterItemImage : public RasterImage
{
public:
    RasterItemImage(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute, const IOOptions &options=IOOptions());
    ~RasterItemImage();

    bool prepare(int prepareType);

protected:
    void setTextureData(ItemTexture * tex, const unsigned int offsetX, const unsigned int offsetY, unsigned int texSizeX, unsigned int texSizeY, unsigned int zoomFactor);
    virtual Texture * GenerateTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor);

private:
    QVector<QRgb> _colorTable;
    static RasterImage *create(DrawerInterface *rootDrawer, const IRasterCoverage &raster, const VisualAttribute &vattribute, const IOOptions &options);

NEW_RASTERIMAGETYPE
};

class ItemTexture : public Texture
{
public:
    ItemTexture(const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor);
    virtual ~ItemTexture();
    virtual void BindMe(QOpenGLShaderProgram & shaders, GLuint texturemat);
    void setQImage(QImage *image, std::vector<quint8> * pixels);
    void setColorTable(QVector<QRgb> & _colorTable);

protected:
    QImage * _image;
    std::vector<quint8> * _pixels;
    bool _recolor;
};
}
}

#endif // RASTERITEMIMAGE_H
