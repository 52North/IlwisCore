#ifndef RASTERVALUEIMAGE_H
#define RASTERVALUEIMAGE_H

#include "drawers/rasterimage.h"

namespace Ilwis{
namespace Geodrawer {

class ValueTexture;

class RasterValueImage : public RasterImage
{
public:
    RasterValueImage(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute, const IOOptions &options=IOOptions());
    ~RasterValueImage();

    bool prepare(int prepareType);

protected:
    void setTextureData(ValueTexture * tex, const unsigned int offsetX, const unsigned int offsetY, unsigned int texSizeX, unsigned int texSizeY, unsigned int zoomFactor);
    virtual Texture * GenerateTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor);

private:
    QVector<QRgb> _colorTable;
    static RasterImage *create(DrawerInterface *rootDrawer, const IRasterCoverage &raster, const VisualAttribute &vattribute, const IOOptions &options);

NEW_RASTERIMAGETYPE
};

class ValueTexture : public Texture
{
public:
    ValueTexture(const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor);
    virtual ~ValueTexture();
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

#endif // RASTERVALUEIMAGE_H
