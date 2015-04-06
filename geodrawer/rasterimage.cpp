#include <QImage>
#include "raster.h"
#include "pixeliterator.h"
#include "rootdrawer.h"
#include "drawers/attributevisualproperties.h"
#include "rasterimage.h"

using namespace Ilwis;
using namespace Geodrawer;

RasterImage::RasterImage(RootDrawer *rootDrawer, const IRasterCoverage& raster, const VisualAttribute& vattribute) :
    _raster(raster), _rootDrawer(rootDrawer), _visualAttribute(vattribute)
{

}

RasterImage::~RasterImage()
{

}

const std::unique_ptr<QImage> &RasterImage::image()
{
    return _image;
}


void RasterImage::makeImage()
{
    BoundingBox bb(_raster->size()); // = _rootDrawer->screenGrf()->coord2Pixel(_raster->envelope());
    quint32 size = bb.xlength() * bb.ylength();
    _pixels.resize(size);
    PixelIterator pixIter(_raster);

    auto end = pixIter.end();
    while(pixIter != end){
        double value = *pixIter;
        auto color = _visualAttribute.value2color(value);
        _pixels[pixIter.linearPosition()] = color;
        ++pixIter;

    }

    const uchar *datablock = (const uchar *)_pixels.data();
    _image.reset(new QImage(datablock,bb.xlength(), bb.ylength(),QImage::Format_RGBA8888));
}



