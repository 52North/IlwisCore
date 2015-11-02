#include <QImage>
#include <QOpenGLTexture>
#include "raster.h"
#include "pixeliterator.h"
#include "drawerinterface.h"
#include "drawers/attributevisualproperties.h"
#include "rasterimage.h"

using namespace Ilwis;
using namespace Geodrawer;

RasterImage::RasterImage(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute, const IOOptions &)
: _raster(raster), _rootDrawer(rootDrawer), _visualAttribute(vattribute)
{
}

RasterImage::~RasterImage()
{
}

void RasterImage::visualAttribute(const VisualAttribute &vattribute)
{
    _visualAttribute = vattribute;
}

bool RasterImage::GetTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor, QOpenGLShaderProgram & shaders, GLuint texturemat) {
    Texture * tex = 0;
    for (int i = 0; i < _textures.size(); ++i) {
        if (_textures[i]->equals(offsetX, offsetY, offsetX + sizeX, offsetY + sizeY, zoomFactor))
            tex = _textures[i];
    }
    if (0 == tex)
        tex = GenerateTexture(offsetX, offsetY, sizeX, sizeY, imgWidth2, imgHeight2, zoomFactor);
    if (tex != 0)
        tex->BindMe(shaders, texturemat);
    return (tex != 0);
}

Texture::Texture(const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor)
: _offsetX(offsetX)
, _offsetY(offsetY)
, _sizeX(sizeX)
, _sizeY(sizeY)
, _imgWidth2(imgWidth2)
, _imgHeight2(imgHeight2)
, _zoomFactor(zoomFactor)
, _texture(0)
{
}

Texture::~Texture()
{
    delete _texture;
}

bool Texture::equals(const long offsetX1, const long offsetY1, const long offsetX2, const long offsetY2, unsigned int zoomFactor)
{
    return this->_offsetX == offsetX1 && this->_offsetY == offsetY1 && this->_offsetX + this->_sizeX == offsetX2 && this->_offsetY + _sizeY == offsetY2 && this->_zoomFactor == zoomFactor;
}

bool Texture::contains(const long offsetX1, const long offsetY1, const long offsetX2, const long offsetY2)
{
    return this->_offsetX <= offsetX1 && this->_offsetY <= offsetY1 && this->_offsetX + _sizeX >= offsetX2 && this->_offsetY + _sizeY >= offsetY2;
}

unsigned int Texture::getZoomFactor()
{
    return _zoomFactor;
}

void Texture::BindMe(QOpenGLShaderProgram &shaders, GLuint texturemat)
{
    _texture->bind();
    double s = _offsetX / (double)_imgWidth2;
    double t = _offsetY / (double)_imgHeight2;
    QMatrix4x4 texmat;
    texmat.setToIdentity();
    texmat.scale(_imgWidth2 / (double)_sizeX, _imgHeight2 / (double)_sizeY, 1);
    texmat.translate(-s, -t, 0);
    shaders.setUniformValue(texturemat, texmat);
}
