#include <QImage>
#include <QOpenGLTexture>
#include "raster.h"
#include "itemdomain.h"
#include "table.h"
#include "pixeliterator.h"
#include "rootdrawer.h"
#include "factory.h"
#include "abstractfactory.h"
#include "drawers/rasterimagefactory.h"
#include "drawers/attributevisualproperties.h"

#include "rasteritemimage.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_RASTERIMAGETYPE(RasterItemImage,itITEMDOMAIN)

RasterItemImage::RasterItemImage(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute, const IOOptions &options)
: RasterImage(rootDrawer, raster, vattribute, options)
{
}

RasterItemImage::~RasterItemImage()
{
}

RasterImage *RasterItemImage::create(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute, const IOOptions &options)
{
    return new RasterItemImage(rootDrawer, raster, vattribute, options)    ;
}

bool RasterItemImage::prepare(int prepareType)
{
    if ( hasType(prepareType, DrawerInterface::ptRENDER)){
        IDomain dom = _raster->datadef().domain();
        auto itemdom = dom.as<ItemDomain<DomainItem>>();
        _colorTable = QVector<QRgb>(itemdom->count() + 1) ;
        _colorTable[0] = 0;
        ITable attTable = _raster->attributeTable();
        for(int i = 1; i < _colorTable.size(); ++i)
            _colorTable[i] = _visualAttribute.value2color(attTable->cell(_visualAttribute.columnIndex(),i)).rgba();
        for (Texture * tex : _textures) {
            ((ItemTexture*)tex)->setColorTable(_colorTable);
        }
    }

    if ( hasType(prepareType, DrawerInterface::ptGEOMETRY)){
    }

    return true;
}

Texture * RasterItemImage::GenerateTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor) {
    ItemTexture * tex = new ItemTexture(offsetX, offsetY, sizeX, sizeY, imgWidth2, imgHeight2, zoomFactor);
    setTextureData(tex, offsetX, offsetY, sizeX, sizeY, zoomFactor);
    _textures.push_back(tex);
    return tex;
}

void RasterItemImage::setTextureData(ItemTexture *tex, const unsigned int offsetX, const unsigned int offsetY, unsigned int texSizeX, unsigned int texSizeY, unsigned int zoomFactor)
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
    texSizeX /= zoomFactor; // the actual size of the texture (commonly 256 or maxtexturesize, but smaller texture sizes may be allocated for the rightmost or bottommost textures)
    texSizeY /= zoomFactor;

    BoundingBox bb(Pixel(offsetX, offsetY), Pixel(offsetX + sizeX - 1, offsetY + sizeY - 1));
    // the "pixels" array must be made 32-bit aligned (32-bit-alignment is required by QImage)
    int xl = (quint32)texSizeX % 4 == 0 ? texSizeX : ((texSizeX / 4) + 1) * 4;
    int rest = xl - texSizeX;
    quint32 size = xl * texSizeY;
    std::vector<quint8> * pixels = new std::vector<quint8> ();
    pixels->resize(size);
    PixelIterator pixIter(_raster, bb); // This iterator runs through bb. The corners of bb are "inclusive".

    auto end = pixIter.end();
    quint32 position = 0;
    while(pixIter != end){
        double value = *pixIter;
        (*pixels)[position] = value == rUNDEF ? 0 : (quint8)value + 1;
        pixIter += zoomFactor;
        if ( pixIter.ychanged()) {
            position += (rest + texSizeX - xSizeOut);
            if (zoomFactor > 1)
                pixIter += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
        }
        ++position;
    }
    const uchar *datablock = (const uchar *)pixels->data();
    QImage * image = new QImage(datablock, xl, texSizeY, QImage::Format_Indexed8);
    image->setColorTable(_colorTable);
    tex->setQImage(image, pixels); // carry over the image and the data-buffer to the texture object (see QImage documentation: the data-buffer must exist throughout the existence of the QImage)
}

ItemTexture::ItemTexture(const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor)
: Texture(offsetX, offsetY, sizeX, sizeY, imgWidth2, imgHeight2, zoomFactor)
, _image(0)
, _pixels(0)
, _recolor(false)
{
}

ItemTexture::~ItemTexture()
{
    delete _image;
    delete _pixels;
}

void ItemTexture::setQImage(QImage * image, std::vector<quint8> *pixels)
{
    if (_image != 0) {
        delete _image;
        delete _pixels;
    }
    _image = image;
    _pixels = pixels;
    if (_texture != 0)
        delete _texture;
    _texture = new QOpenGLTexture(*(_image));
    _texture->setMinMagFilters(QOpenGLTexture::Nearest,QOpenGLTexture::Nearest);
    _texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    _recolor = false;
}

void ItemTexture::setColorTable(QVector<QRgb> & _colorTable)
{
    if (_image != 0) {
        _image->setColorTable(_colorTable);
        _recolor = true;
    }
}

void ItemTexture::BindMe(QOpenGLShaderProgram &shaders, GLuint texturemat)
{
    if (_recolor && _image != 0) {
        if (_texture)
            delete _texture;
        _texture = new QOpenGLTexture(*(_image));
        _texture->setMinMagFilters(QOpenGLTexture::Nearest,QOpenGLTexture::Nearest);
        _texture->setWrapMode(QOpenGLTexture::ClampToEdge);
        _recolor = false;
    }
    Texture::BindMe(shaders, texturemat);
}




