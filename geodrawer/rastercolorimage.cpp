#include <QImage>
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

RasterColorImage::RasterColorImage(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute,const IOOptions &options) :
    RasterImage(rootDrawer,raster,vattribute, options)
{

}

RasterColorImage::~RasterColorImage()
{

}

RasterImage *RasterColorImage::create(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute,const IOOptions &options)
{
    return new RasterColorImage(rootDrawer, raster, vattribute, options)    ;
}

bool RasterColorImage::prepare(int prepareType)
{
    bool resetImage = true;
    BoundingBox bb(_raster->size()); // = _rootDrawer->screenGrf()->coord2Pixel(_raster->envelope());
    if ( hasType(prepareType, DrawerInterface::ptGEOMETRY)){
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
        resetImage = true;
    }
    if ( resetImage){
        const uchar *datablock = (const uchar *)_pixels.data();
        _image.reset(new QImage(datablock,bb.xlength(), bb.ylength(),QImage::Format_RGBA8888));
    }
    return resetImage;
}

