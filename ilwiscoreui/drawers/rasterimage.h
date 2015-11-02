#ifndef RASTERIMAGE_H
#define RASTERIMAGE_H

#include "ilwiscoreui_global.h"
#include <QOpenGLShaderProgram>

class QImage;

namespace Ilwis{
namespace Geodrawer {
class RootDrawer;
class DrawerInterface;
class Texture;

class ILWISCOREUISHARED_EXPORT RasterImage
{
public:
    RasterImage(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute, const IOOptions &options=IOOptions());
    virtual ~RasterImage();

    virtual bool prepare(int prepareType) = 0;
    void visualAttribute(const VisualAttribute &vattribute);
    bool GetTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor, QOpenGLShaderProgram & shaders, GLuint texturemat);

protected:
    virtual Texture * GenerateTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor) = 0;
    std::vector<Texture*> _textures;
    IRasterCoverage _raster;
    DrawerInterface *_rootDrawer;
    VisualAttribute _visualAttribute;

private:
};

class ILWISCOREUISHARED_EXPORT Texture
{
public:
    Texture(const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor);
    virtual ~Texture();
    virtual void BindMe(QOpenGLShaderProgram & shaders, GLuint texturemat);
    bool equals(const long offsetX1, const long offsetY1, const long offsetX2, const long offsetY2, unsigned int zoomFactor);
    bool contains(const long offsetX1, const long offsetY1, const long offsetX2, const long offsetY2);
    unsigned int getZoomFactor();

protected:
    const long _offsetX, _offsetY;
    const unsigned long _sizeX, _sizeY;
    const unsigned long _imgWidth2, _imgHeight2;
    const unsigned int _zoomFactor;
    QOpenGLTexture * _texture;
};

#define NEW_RASTERIMAGETYPE \
    private:\
        static RasterImage *dummy_raster_image;

#define REGISTER_RASTERIMAGETYPE(classname,tp) \
    RasterImage *classname::dummy_raster_image = RasterImageFactory::registerRasterImageType(#classname,tp,classname::create);
}
}

#endif // RASTERIMAGE_H
