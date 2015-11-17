#include <QImage>
#include <QOpenGLTexture>
#include "raster.h"
#include "pixeliterator.h"
#include "rootdrawer.h"
#include "factory.h"
#include "abstractfactory.h"
#include "drawers/rasterimagefactory.h"
#include "drawers/attributevisualproperties.h"
#include "rastercolorimage.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_RASTERIMAGETYPE(RasterColorImage,itCOLORDOMAIN)

RasterColorImage::RasterColorImage(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute, const IOOptions &options)
: RasterImage(rootDrawer,raster,vattribute, options)
{
}

RasterColorImage::~RasterColorImage()
{
}

RasterImage *RasterColorImage::create(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute, const IOOptions &options)
{
    return new RasterColorImage(rootDrawer, raster, vattribute, options)    ;
}

bool RasterColorImage::prepare(int prepareType)
{
    if ( hasType(prepareType, DrawerInterface::ptGEOMETRY)){
    }
    return true;
}

Texture * RasterColorImage::GenerateTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor) {
    RGBTexture * tex = new RGBTexture(offsetX, offsetY, sizeX, sizeY, imgWidth2, imgHeight2, zoomFactor);
    setTextureData(tex, offsetX, offsetY, sizeX, sizeY, zoomFactor);
    _textures.push_back(tex);
    return tex;
}

void RasterColorImage::setTextureData(RGBTexture *tex, const unsigned int offsetX, const unsigned int offsetY, unsigned int texSizeX, unsigned int texSizeY, unsigned int zoomFactor)
{
    long imageWidth = _raster->size().xsize();
    long imageHeight = _raster->size().ysize();
    long sizeX = texSizeX; // the size of the input (pixeliterator)
    long sizeY = texSizeY;
    if (offsetX + sizeX > imageWidth)
        sizeX = imageWidth - offsetX;
    if (offsetY + sizeY > imageHeight)
        sizeY = imageHeight - offsetY;
    if (sizeX == 0 || sizeY == 0)
        return;
    const long xSizeOut = (long)ceil((double)sizeX / ((double)zoomFactor)); // the size until which the pixels vector will be filled (this is commonly the same as texSizeX, except the rightmost / bottommost textures, as raster-images seldom have as size of ^2)
    const long ySizeOut = (long)ceil((double)sizeY / ((double)zoomFactor));
    texSizeX /= zoomFactor; // the actual size of the texture (commonly 256 or maxtexturesize, but smaller texture sizes may be allocated for the rightmost or bottommost textures)
    texSizeY /= zoomFactor;

    BoundingBox bb(Pixel(offsetX, offsetY), Pixel(offsetX + sizeX - 1, offsetY + sizeY - 1));
    quint32 size = texSizeX * texSizeY;
    std::vector<VertexColorI> pixels; // this array is automatically 32-bit aligned due to VertexColorI (32-bit-alignment is required by QImage)
    pixels.resize(size);
    PixelIterator pixIter(_raster, bb); // This iterator runs through bb. The corners of bb are "inclusive".

    auto end = pixIter.end();
    quint32 position = 0;
    while(pixIter != end){
        double value = *pixIter;
        auto color = _visualAttribute.value2color(value);
        pixels[position] = color;
        pixIter += zoomFactor;
        if ( pixIter.ychanged()) {
            position += (texSizeX - xSizeOut);
            if (zoomFactor > 1)
                pixIter += sizeX * (zoomFactor - 1);
        }
        ++position;
    }
    const uchar *datablock = (const uchar *)pixels.data();
    QImage image (datablock, texSizeX, texSizeY, QImage::Format_RGBA8888);
    QOpenGLTexture * texture = new QOpenGLTexture(image);
    texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
    texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    tex->setTexture(texture);
}

RGBTexture::RGBTexture(const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor)
: Texture(offsetX, offsetY, sizeX, sizeY, imgWidth2, imgHeight2, zoomFactor)
{
}

RGBTexture::~RGBTexture()
{
}

void RGBTexture::setTexture(QOpenGLTexture * texture)
{
    if (_texture != 0)
        delete _texture;
    _texture = texture;
}

