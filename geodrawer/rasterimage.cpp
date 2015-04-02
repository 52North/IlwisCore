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
    Size<> rastersize = _raster->size();
    BoundingBox bb = _rootDrawer->screenGrf()->coord2Pixel(_raster->envelope());
    quint32 size = bb.xlength() * bb.ylength();
    _pixels.resize(size);
    PixelIterator pixIter(_raster);
     PixelIterator pixIter2(_raster);
     auto end2 = pixIter.end();
     int xblockSize = bb.xlength() / rastersize.xsize();
     int yblockSize = bb.ylength() / rastersize.ysize();
     int indexBase = 0;
     int lastXBlockStart = 0;
     while(pixIter2 != end2){
         double value = *pixIter2;
         auto color = _visualAttribute.value2color(value);
         for(int y = 0; y < yblockSize; ++y){
             for(int x = 0; x < xblockSize; ++x){
                 _pixels[indexBase + x] = color;
                 ++indexBase;
             }
             indexBase += bb.xlength() - xblockSize;
         }
         ++pixIter2;
         if ( pixIter2.ychanged()){
            indexBase = indexBase + xblockSize + 1;
         }else
            indexBase = lastXBlockStart + xblockSize;
         lastXBlockStart = indexBase;

     }
     const uchar *datablock = (const uchar *)_pixels.data();
    _image.reset(new QImage(datablock,bb.xlength(), bb.ylength(),QImage::Format_RGBA8888));
}



